﻿#include "HUD.h"
#include "ControlScheme.h"
#include "../LevelHandler.h"
#include "../PreferencesCache.h"
#include "../Actors/Enemies/Bosses/BossBase.h"

#include "Graphics/RenderQueue.h"
#include "Base/Random.h"
#include "Application.h"

// Position of key in 22x6 grid
static constexpr uint8_t KeyLayout[] = {
	0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17,
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 38, 39, 40, 41, 42, 43,
	44, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
	66, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 84, 85, 86,
	88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 101, 104, 106, 107, 108, 109,
	110, 111, 112, 116, 120, 121, 122, 123, 125, 126, 127, 128, 130
};

namespace Jazz2::UI
{
	HUD::HUD(LevelHandler* levelHandler)
		: _levelHandler(levelHandler), _graphics(nullptr), _levelTextTime(-1.0f), _coins(0), _gems(0), _coinsTime(-1.0f), _gemsTime(-1.0f),
			_activeBossTime(0.0f), _touchButtonsTimer(0.0f), _rgbAmbientLight(0.0f), _rgbHealthLast(0.0f), _weaponWheelAnim(0.0f),
			_weaponWheelShown(false), _lastWeaponWheelIndex(-1), _rgbLightsTime(0.0f), _transitionState(TransitionState::None),
			_transitionTime(0.0f)
	{
		auto& resolver = ContentResolver::Get();

		Metadata* metadata = resolver.RequestMetadata("UI/HUD"_s);
		if (metadata != nullptr) {
			_graphics = &metadata->Graphics;
		}

		_smallFont = resolver.GetFont(FontType::Small);

		_touchButtons[0] = CreateTouchButton(PlayerActions::None, "TouchDpad"_s, Alignment::BottomLeft, DpadLeft, DpadBottom, DpadSize, DpadSize);
		// D-pad subsections
		_touchButtons[1] = CreateTouchButton(PlayerActions::Up, nullptr, Alignment::BottomLeft, DpadLeft, DpadBottom + (DpadSize * 2 / 3), DpadSize, (DpadSize / 3) + DpadThreshold);
		_touchButtons[2] = CreateTouchButton(PlayerActions::Down, nullptr, Alignment::BottomLeft, DpadLeft, DpadBottom - DpadThreshold, DpadSize, (DpadSize / 3) + DpadThreshold);
		_touchButtons[3] = CreateTouchButton(PlayerActions::Left, nullptr, Alignment::BottomLeft | AllowRollover, DpadLeft - DpadThreshold, DpadBottom, (DpadSize / 3) + DpadThreshold, DpadSize);
		_touchButtons[4] = CreateTouchButton(PlayerActions::Right, nullptr, Alignment::BottomLeft | AllowRollover, DpadLeft + (DpadSize * 2 / 3), DpadBottom, (DpadSize / 3) + DpadThreshold, DpadSize);
		// Action buttons
		_touchButtons[5] = CreateTouchButton(PlayerActions::Fire, "TouchFire"_s, Alignment::BottomRight, (ButtonSize + 0.02f) * 2, 0.04f, ButtonSize, ButtonSize);
		_touchButtons[6] = CreateTouchButton(PlayerActions::Jump, "TouchJump"_s, Alignment::BottomRight, (ButtonSize + 0.02f), 0.04f + 0.08f, ButtonSize, ButtonSize);
		_touchButtons[7] = CreateTouchButton(PlayerActions::Run, "TouchRun"_s, Alignment::BottomRight, 0.001f, 0.01f + 0.15f, ButtonSize, ButtonSize);
		_touchButtons[8] = CreateTouchButton(PlayerActions::ChangeWeapon, "TouchChange"_s, Alignment::BottomRight, ButtonSize + 0.01f, 0.04f + 0.28f, SmallButtonSize, SmallButtonSize);
		_touchButtons[9] = CreateTouchButton(PlayerActions::Menu, "TouchPause"_s, Alignment::TopRight | Fixed, 0.02f, 0.02f, SmallButtonSize, SmallButtonSize);
	}

	HUD::~HUD()
	{
#if !defined(DEATH_TARGET_ANDROID) && !defined(DEATH_TARGET_IOS) && !defined(DEATH_TARGET_WINDOWS_RT)
		if (PreferencesCache::EnableRgbLights) {
			RgbLights::Get().Clear();
		}
#endif
	}

	void HUD::OnUpdate(float timeMult)
	{
		Canvas::OnUpdate(timeMult);

		if (_levelTextTime >= 0.0f) {
			_levelTextTime += timeMult;
		}
		if (_touchButtonsTimer > 0.0f) {
			_touchButtonsTimer -= timeMult;
		}

		switch (_transitionState) {
			case TransitionState::FadeIn:
				_transitionTime += 0.025f * timeMult;
				if (_transitionTime >= 1.0f) {
					_transitionState = TransitionState::None;
				}
				break;
			case TransitionState::FadeOut:
				if (_transitionTime > 0.0f) {
					_transitionTime -= 0.025f * timeMult;
					if (_transitionTime < 0.0f) {
						_transitionTime = 0.0f;
					}
				}
				break;
			case TransitionState::WaitingForFadeOut:
				_transitionTime -= timeMult;
				if (_transitionTime <= 0.0f) {
					_transitionState = TransitionState::FadeOut;
					_transitionTime = 1.0f;
				}
				break;
		}

		auto& players = _levelHandler->GetPlayers();
		if (!players.empty()) {
			if (_coinsTime >= 0.0f) {
				_coinsTime += timeMult;
			}
			if (_gemsTime >= 0.0f) {
				_gemsTime += timeMult;
			}
			if (_levelHandler->_activeBoss != nullptr) {
				_activeBossTime += timeMult;

				constexpr float TransitionTime = 60.0f;
				if (_activeBossTime > TransitionTime) {
					_activeBossTime = TransitionTime;
				}
			} else {
				_activeBossTime = 0.0f;
			}

			if (PrepareWeaponWheel(players[0], _weaponWheelCount)) {
				if (_weaponWheelAnim < WeaponWheelAnimDuration) {
					_weaponWheelAnim += timeMult;
					if (_weaponWheelAnim > WeaponWheelAnimDuration) {
						_weaponWheelAnim = WeaponWheelAnimDuration;
					}
				}
			} else {
				if (_weaponWheelAnim > 0.0f) {
					_weaponWheelAnim -= timeMult * 2.0f;
					if (_weaponWheelAnim <= 0.0f) {
						_weaponWheelAnim = 0.0f;
						_levelHandler->_playerFrozenEnabled = false;

						Actors::Player* player = players[0];
						if (player->_weaponWheelState == Actors::Player::WeaponWheelState::Visible) {
							player->_weaponWheelState = Actors::Player::WeaponWheelState::Closing;
						}
					}
				}
			}

			UpdateRgbLights(timeMult, players[0]);
		}
	}

	bool HUD::OnDraw(RenderQueue& renderQueue)
	{
		Canvas::OnDraw(renderQueue);

		if (_graphics == nullptr) {
			return false;
		}

		ViewSize = _levelHandler->GetViewSize();

		Rectf view = Rectf(0.0f, 0.0f, static_cast<float>(ViewSize.X), static_cast<float>(ViewSize.Y));
		Rectf adjustedView = view;
		if (_touchButtonsTimer > 0.0f) {
			adjustedView.X = 140.0f + PreferencesCache::TouchLeftPadding.X;
			adjustedView.W = adjustedView.W - adjustedView.X - (195.0f + PreferencesCache::TouchRightPadding.X);
		}

		float right = adjustedView.X + adjustedView.W;
		float bottom = adjustedView.Y + adjustedView.H;

		int32_t charOffset = 0;
		int32_t charOffsetShadow = 0;
		char stringBuffer[32];

		auto& players = _levelHandler->GetPlayers();
		if (!players.empty()) {
			Actors::Player* player = players[0];
			PlayerType playerType = player->_playerType;

			// Bottom left
			StringView playerIcon;
			switch (playerType) {
				default:
				case PlayerType::Jazz: playerIcon = "CharacterJazz"_s; break;
				case PlayerType::Spaz: playerIcon = "CharacterSpaz"_s; break;
				case PlayerType::Lori: playerIcon = "CharacterLori"_s; break;
				case PlayerType::Frog: playerIcon = "CharacterFrog"_s; break;
			}

			DrawElement(playerIcon, -1, adjustedView.X + 38.0f, bottom - 1.0f + 1.6f, ShadowLayer, Alignment::BottomRight, Colorf(0.0f, 0.0f, 0.0f, 0.4f));
			DrawElement(playerIcon, -1, adjustedView.X + 38.0f, bottom - 1.0f, MainLayer, Alignment::BottomRight, Colorf::White);

			if (_levelHandler->IsReforged()) {
				for (int32_t i = 0; i < player->_health; i++) {
					stringBuffer[i] = '|';
				}
				stringBuffer[player->_health] = '\0';

				if (player->_lives > 0) {
					_smallFont->DrawString(this, stringBuffer, charOffsetShadow, adjustedView.X + 36.0f - 3.0f - 0.5f, bottom - 17.0f + 0.5f, FontShadowLayer,
						Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.42f), 0.7f, 0.0f, 0.0f, 0.0f, 0.0f, 1.1f);
					_smallFont->DrawString(this, stringBuffer, charOffsetShadow, adjustedView.X + 36.0f - 3.0f + 0.5f, bottom - 17.0f - 0.5f, FontShadowLayer,
						Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.42f), 0.7f, 0.0f, 0.0f, 0.0f, 0.0f, 1.1f);
					_smallFont->DrawString(this, stringBuffer, charOffset, adjustedView.X + 36.0f - 3.0f, bottom - 17.0f, FontLayer,
						Alignment::BottomLeft, Font::RandomColor, 0.7f, 0.0f, 0.0f, 0.0f, 0.0f, 1.1f);

					if (player->_lives < UINT8_MAX) {
						stringBuffer[0] = 'x';
						i32tos(player->_lives, stringBuffer + 1);
						
						_smallFont->DrawString(this, stringBuffer, charOffsetShadow, adjustedView.X + 36.0f - 4.0f, bottom - 1.0f + 1.0f, FontShadowLayer,
							Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.32f));
						_smallFont->DrawString(this, stringBuffer, charOffset, adjustedView.X + 36.0f - 4.0f, bottom - 1.0f, FontLayer,
							Alignment::BottomLeft, Font::DefaultColor);
					} else {
						_smallFont->DrawString(this, "x\u221E", charOffsetShadow, adjustedView.X + 36.0f - 4.0f, bottom - 1.0f + 1.0f, FontShadowLayer,
							Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.32f));
						_smallFont->DrawString(this, "x\u221E", charOffset, adjustedView.X + 36.0f - 4.0f, bottom - 1.0f, FontLayer,
							Alignment::BottomLeft, Font::DefaultColor);
					}
				} else {
					_smallFont->DrawString(this, stringBuffer, charOffsetShadow, adjustedView.X + 36.0f - 3.0f - 0.5f, bottom - 4.0f + 0.5f, FontShadowLayer,
						Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.42f), 0.7f, 0.0f, 0.0f, 0.0f, 0.0f, 1.1f);
					_smallFont->DrawString(this, stringBuffer, charOffsetShadow, adjustedView.X + 36.0f - 3.0f + 0.5f, bottom - 4.0f - 0.5f, FontShadowLayer,
						Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.42f), 0.7f, 0.0f, 0.0f, 0.0f, 0.0f, 1.1f);
					_smallFont->DrawString(this, stringBuffer, charOffset, adjustedView.X + 36.0f - 3.0f, bottom - 4.0f, FontLayer,
						Alignment::BottomLeft, Font::RandomColor, 0.7f, 0.0f, 0.0f, 0.0f, 0.0f, 1.1f);
				}

				// Top left
				DrawElement("PickupFood"_s, -1, view.X + 3.0f, view.Y + 3.0f + 1.6f, ShadowLayer, Alignment::TopLeft, Colorf(0.0f, 0.0f, 0.0f, 0.4f));
				DrawElement("PickupFood"_s, -1, view.X + 3.0f, view.Y + 3.0f, MainLayer, Alignment::TopLeft, Colorf::White);

				snprintf(stringBuffer, countof(stringBuffer), "%08i", player->_score);
				_smallFont->DrawString(this, stringBuffer, charOffsetShadow, view.X + 14.0f, view.Y + 5.0f + 1.0f, FontShadowLayer,
					Alignment::TopLeft, Colorf(0.0f, 0.0f, 0.0f, 0.32f), 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.88f);
				_smallFont->DrawString(this, stringBuffer, charOffset, view.X + 14.0f, view.Y + 5.0f, FontLayer,
					Alignment::TopLeft, Font::DefaultColor, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.88f);
			} else {
				for (int32_t i = 0; i < player->_health; i++) {
					DrawElement("Heart"_s, -1, view.X + view.W - 4.0f - (i * 16.0f), view.Y + 4.0f, MainLayer, Alignment::TopRight, Colorf::White);
				}

				if (player->_lives > 0) {
					if (player->_lives < UINT8_MAX) {
						stringBuffer[0] = 'x';
						i32tos(player->_lives, stringBuffer + 1);
						_smallFont->DrawString(this, stringBuffer, charOffsetShadow, adjustedView.X + 36.0f - 4.0f, bottom - 1.0f + 1.0f, FontShadowLayer,
							Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.32f));
						_smallFont->DrawString(this, stringBuffer, charOffset, adjustedView.X + 36.0f - 4.0f, bottom - 1.0f, FontLayer,
							Alignment::BottomLeft, Font::DefaultColor);
					} else {
						_smallFont->DrawString(this, "x\u221E", charOffsetShadow, adjustedView.X + 36.0f - 4.0f, bottom - 1.0f + 1.0f, FontShadowLayer,
							Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.32f));
						_smallFont->DrawString(this, "x\u221E", charOffset, adjustedView.X + 36.0f - 4.0f, bottom - 1.0f, FontLayer,
							Alignment::BottomLeft, Font::DefaultColor);
					}
				}

				snprintf(stringBuffer, countof(stringBuffer), "%08i", player->_score);
				_smallFont->DrawString(this, stringBuffer, charOffsetShadow, view.X + 4.0f, view.Y + 1.0f + 1.0f, FontShadowLayer,
					Alignment::TopLeft, Colorf(0.0f, 0.0f, 0.0f, 0.32f), 1.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.88f);
				_smallFont->DrawString(this, stringBuffer, charOffset, view.X + 4.0f, view.Y + 1.0f, FontLayer,
					Alignment::TopLeft, Font::DefaultColor, 1.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.88f);
			}

			// Bottom right
			if (player->_weaponAllowed && playerType != PlayerType::Frog) {
				WeaponType weapon = player->_currentWeapon;
				Vector2f pos = Vector2f(right - 40.0f, bottom - 2.0f);
				StringView currentWeaponString = GetCurrentWeapon(player, weapon, pos);

				StringView ammoCount;
				if (player->_weaponAmmo[(int32_t)weapon] == UINT16_MAX) {
					ammoCount = "x\u221E"_s;
				} else {
					stringBuffer[0] = 'x';
					i32tos(player->_weaponAmmo[(int32_t)weapon] / 256, stringBuffer + 1);
					ammoCount = stringBuffer;
				}
				_smallFont->DrawString(this, ammoCount, charOffsetShadow, right - 40.0f, bottom - 2.0f + 1.0f, FontShadowLayer,
					Alignment::BottomLeft, Colorf(0.0f, 0.0f, 0.0f, 0.32f), 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.96f);
				_smallFont->DrawString(this, ammoCount, charOffset, right - 40.0f, bottom - 2.0f, FontLayer,
					Alignment::BottomLeft, Font::DefaultColor, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.96f);

				auto it = _graphics->find(String::nullTerminatedView(currentWeaponString));
				if (it != _graphics->end()) {
					if (it->second.Base->FrameDimensions.Y < 20) {
						pos.Y -= std::round((20 - it->second.Base->FrameDimensions.Y) * 0.5f);
					}

					DrawElement(currentWeaponString, -1, pos.X, pos.Y + 1.6f, ShadowLayer, Alignment::BottomRight, Colorf(0.0f, 0.0f, 0.0f, 0.4f));
					DrawElement(currentWeaponString, -1, pos.X, pos.Y, MainLayer, Alignment::BottomRight, Colorf::White);
				}
			}

			// Active Boss (health bar)
			if (_levelHandler->_activeBoss != nullptr && _levelHandler->_activeBoss->GetMaxHealth() != INT32_MAX) {
				constexpr float TransitionTime = 60.0f;
				float y, alpha;
				if (_activeBossTime < TransitionTime) {
					y = (TransitionTime - _activeBossTime) / 8.0f;
					y = bottom * 0.1f - (y * y);
					alpha = std::max(_activeBossTime / TransitionTime, 0.0f);
				} else {
					y = bottom * 0.1f;
					alpha = 1.0f;
				}

				float perc = 0.08f + 0.84f * _levelHandler->_activeBoss->GetHealth() / _levelHandler->_activeBoss->GetMaxHealth();

				DrawElement("BossHealthBar"_s, 0, ViewSize.X * 0.5f, y + 2.0f, ShadowLayer, Alignment::Center, Colorf(0.0f, 0.0f, 0.0f, 0.1f * alpha));
				DrawElement("BossHealthBar"_s, 0, ViewSize.X * 0.5f, y + 1.0f, ShadowLayer, Alignment::Center, Colorf(0.0f, 0.0f, 0.0f, 0.2f * alpha));

				DrawElement("BossHealthBar"_s, 0, ViewSize.X * 0.5f, y, MainLayer, Alignment::Center, Colorf(1.0f, 1.0f, 1.0f, alpha));
				DrawElementClipped("BossHealthBar"_s, 1, ViewSize.X * 0.5f, y, MainLayer + 2, Alignment::Center, Colorf(1.0f, 1.0f, 1.0f, alpha), perc, 1.0f);
			}

			// Misc
			DrawLevelText(charOffset);
			DrawCoins(charOffset);
			DrawGems(charOffset);

			DrawWeaponWheel(player);

			// FPS
			if (PreferencesCache::ShowPerformanceMetrics) {
				i32tos((int32_t)std::round(theApplication().averageFps()), stringBuffer);
				_smallFont->DrawString(this, stringBuffer, charOffset, view.W - 4.0f, view.Y + 2.0f, FontLayer,
					Alignment::TopRight, Font::DefaultColor, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.96f);
			}

			// Touch Controls
			if (_touchButtonsTimer > 0.0f) {
				for (auto& button : _touchButtons) {
					if (button.Graphics == nullptr || (PreferencesCache::UseNativeBackButton && button.Action == PlayerActions::Menu)) {
						continue;
					}

					float x = button.Left;
					float y = button.Top;
					if ((button.Align & Alignment::Right) == Alignment::Right) {
						x = ViewSize.X - button.Width * 0.5f - x;
					} else {
						x = x + button.Width * 0.5f;
					}
					if ((button.Align & Alignment::Bottom) == Alignment::Bottom) {
						y = ViewSize.Y - button.Height * 0.5f - y;
					} else {
						y = y + button.Height * 0.5f;
					}
					if ((button.Align & Fixed) != Fixed) {
						if ((button.Align & Alignment::Right) == Alignment::Right) {
							x -= PreferencesCache::TouchRightPadding.X;
							y += PreferencesCache::TouchRightPadding.Y;
						} else {
							x += PreferencesCache::TouchLeftPadding.X;
							y += PreferencesCache::TouchLeftPadding.Y;
						}
					}
					x = x - ViewSize.X * 0.5f;
					y = ViewSize.Y * 0.5f - y;

					DrawTexture(*button.Graphics->Base->TextureDiffuse, Vector2f(x, y), TouchButtonsLayer, Vector2f(button.Width, button.Height), Vector4f(1.0f, 0.0f, -1.0f, 1.0f), Colorf::White);
				}
			}
		}

		if (_transitionState == TransitionState::FadeIn || _transitionState == TransitionState::FadeOut) {
			auto command = RentRenderCommand();
			if (command->material().setShader(ContentResolver::Get().GetShader(PrecompiledShader::Transition))) {
				command->material().reserveUniformsDataMemory();
				command->geometry().setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);
			}

			command->material().setBlendingFactors(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			auto instanceBlock = command->material().uniformBlock(Material::InstanceBlockName);
			instanceBlock->uniform(Material::TexRectUniformName)->setFloatVector(Vector4f(1.0f, 0.0f, 1.0f, 0.0f).Data());
			instanceBlock->uniform(Material::SpriteSizeUniformName)->setFloatVector(Vector2f(static_cast<float>(ViewSize.X), static_cast<float>(ViewSize.Y)).Data());
			instanceBlock->uniform(Material::ColorUniformName)->setFloatVector(Colorf(0.0f, 0.0f, 0.0f, _transitionTime).Data());

			command->setTransformation(Matrix4x4f::Identity);
			command->setLayer(999);

			renderQueue.addCommand(command);
		}

		return true;
	}

	void HUD::OnTouchEvent(const TouchEvent& event, uint32_t& overrideActions)
	{
		_touchButtonsTimer = 1200.0f;

		if (event.type == TouchEventType::Down || event.type == TouchEventType::PointerDown) {
			int32_t pointerIndex = event.findPointerIndex(event.actionIndex);
			if (pointerIndex != -1) {
				float x = event.pointers[pointerIndex].x * (float)ViewSize.X;
				float y = event.pointers[pointerIndex].y * (float)ViewSize.Y;
				for (uint32_t i = 0; i < TouchButtonsCount; i++) {
					auto& button = _touchButtons[i];
					if (button.Action != PlayerActions::None) {
						if (button.CurrentPointerId == -1 && IsOnButton(button, x, y)) {
							button.CurrentPointerId = event.actionIndex;
							overrideActions |= (1 << (int32_t)button.Action);
						}
					}
				}
			}
		} else if (event.type == TouchEventType::Move) {
			for (uint32_t i = 0; i < TouchButtonsCount; i++) {
				auto& button = _touchButtons[i];
				if (button.Action != PlayerActions::None) {
					if (button.CurrentPointerId != -1) {
						bool isPressed = false;
						int32_t pointerIndex = event.findPointerIndex(button.CurrentPointerId);
						if (pointerIndex != -1) {
							float x = event.pointers[pointerIndex].x * (float)ViewSize.X;
							float y = event.pointers[pointerIndex].y * (float)ViewSize.Y;
							isPressed = IsOnButton(button, x, y);
						}

						if (!isPressed) {
							button.CurrentPointerId = -1;
							overrideActions &= ~(1 << (int32_t)button.Action);
						}
					} else {
						// Only some buttons should allow roll-over (only when the player's on foot)
						auto& players = _levelHandler->GetPlayers();
						bool canPlayerMoveVertically = (!players.empty() && players[0]->CanMoveVertically());
						if ((button.Align & AllowRollover) != AllowRollover && !canPlayerMoveVertically) continue;

						for (uint32_t j = 0; j < event.count; j++) {
							float x = event.pointers[j].x * (float)ViewSize.X;
							float y = event.pointers[j].y * (float)ViewSize.Y;
							if (IsOnButton(button, x, y)) {
								button.CurrentPointerId = event.pointers[j].id;
								overrideActions |= (1 << (int32_t)button.Action);
								break;
							}
						}
					}
				}
			}
		} else if (event.type == TouchEventType::Up) {
			for (uint32_t i = 0; i < TouchButtonsCount; i++) {
				auto& button = _touchButtons[i];
				if (button.CurrentPointerId != -1) {
					button.CurrentPointerId = -1;
					overrideActions &= ~(1 << (int32_t)button.Action);
				}
			}

		} else if (event.type == TouchEventType::PointerUp) {
			for (uint32_t i = 0; i < TouchButtonsCount; i++) {
				auto& button = _touchButtons[i];
				if (button.CurrentPointerId == event.actionIndex) {
					button.CurrentPointerId = -1;
					overrideActions &= ~(1 << (int32_t)button.Action);
				}
			}
		}
	}

	void HUD::ShowLevelText(const StringView& text)
	{
		if (_levelText == text || text.empty()) {
			return;
		}

		_levelText = text;
		_levelTextTime = 0.0f;
	}

	void HUD::ShowCoins(int32_t count)
	{
		constexpr float StillTime = 120.0f;
		constexpr float TransitionTime = 60.0f;

		_coins = count;

		if (_coinsTime < 0.0f) {
			_coinsTime = 0.0f;
		} else if (_coinsTime > TransitionTime) {
			_coinsTime = TransitionTime;
		}

		if (_gemsTime >= 0.0f) {
			if (_gemsTime <= TransitionTime + StillTime) {
				_gemsTime = TransitionTime + StillTime;
			} else {
				_gemsTime = -1.0f;
			}
		}
	}

	void HUD::ShowGems(int32_t count)
	{
		constexpr float StillTime = 120.0f;
		constexpr float TransitionTime = 60.0f;

		_gems = count;

		if (_gemsTime < 0.0f) {
			_gemsTime = 0.0f;
		} else if (_gemsTime > TransitionTime) {
			_gemsTime = TransitionTime;
		}

		if (_coinsTime >= 0.0f) {
			if (_coinsTime <= TransitionTime + StillTime) {
				_coinsTime = TransitionTime + StillTime;
			} else {
				_coinsTime = -1.0f;
			}
		}
	}

	void HUD::BeginFadeIn()
	{
		_transitionState = TransitionState::FadeIn;
		_transitionTime = 0.0f;
	}

	void HUD::BeginFadeOut(float delay)
	{
		if (delay <= 0.0f) {
			_transitionState = TransitionState::FadeOut;
			_transitionTime = 1.0f;
		} else {
			_transitionState = TransitionState::WaitingForFadeOut;
			_transitionTime = delay;
		}
	}

	void HUD::DrawLevelText(int32_t& charOffset)
	{
		constexpr float StillTime = 350.0f;
		constexpr float TransitionTime = 100.0f;
		constexpr float TotalTime = StillTime + TransitionTime * 2.0f;

		if (_levelTextTime < 0.0f) {
			return;
		}

		float offset;
		if (_levelTextTime < TransitionTime) {
			offset = powf((TransitionTime - _levelTextTime) / 12.0f, 3);
		} else if (_levelTextTime > TransitionTime + StillTime) {
			offset = -powf((_levelTextTime - TransitionTime - StillTime) / 12.0f, 3);
		} else {
			offset = 0;
		}

		int32_t charOffsetShadow = charOffset;
		_smallFont->DrawString(this, _levelText, charOffsetShadow, ViewSize.X * 0.5f + offset, ViewSize.Y * 0.04f + 2.5f, FontShadowLayer,
			Alignment::Top, Colorf(0.0f, 0.0f, 0.0f, 0.3f), 1.0f, 0.72f, 0.8f, 0.8f);

		_smallFont->DrawString(this, _levelText, charOffset, ViewSize.X * 0.5f + offset, ViewSize.Y * 0.04f, FontLayer,
			Alignment::Top, Font::DefaultColor, 1.0f, 0.72f, 0.8f, 0.8f);

		if (_levelTextTime > TotalTime) {
			_levelTextTime = -1.0f;
			_levelText = { };
		}
	}

	void HUD::DrawCoins(int32_t& charOffset)
	{
		constexpr float StillTime = 120.0f;
		constexpr float TransitionTime = 60.0f;
		constexpr float TotalTime = StillTime + TransitionTime * 2.0f;

		if (_coinsTime < 0.0f) {
			return;
		}

		float offset, alpha;
		if (_coinsTime < TransitionTime) {
			offset = (TransitionTime - _coinsTime) / 10.0f;
			offset = -(offset * offset);
			alpha = std::max(_coinsTime / TransitionTime, 0.1f);
		} else if (_coinsTime > TransitionTime + StillTime) {
			offset = (_coinsTime - TransitionTime - StillTime) / 10.0f;
			offset = (offset * offset);
			alpha = (TotalTime - _coinsTime) / TransitionTime;
		} else {
			offset = 0.0f;
			alpha = 1.0f;
		}

		DrawElement("PickupCoin"_s, -1, ViewSize.X * 0.5f, ViewSize.Y * 0.92f + 2.5f + offset, ShadowLayer,
			Alignment::Right, Colorf(0.0f, 0.0f, 0.0f, 0.2f * alpha), 0.8f, 0.8f);
		DrawElement("PickupCoin"_s, -1, ViewSize.X * 0.5f, ViewSize.Y * 0.92f + offset, MainLayer,
			Alignment::Right, Colorf(1.0f, 1.0f, 1.0f, alpha * alpha), 0.8f, 0.8f);

		char stringBuffer[32];
		snprintf(stringBuffer, countof(stringBuffer), "x%i", _coins);

		int32_t charOffsetShadow = charOffset;
		_smallFont->DrawString(this, stringBuffer, charOffsetShadow, ViewSize.X * 0.5f, ViewSize.Y * 0.92f + 2.5f + offset, FontShadowLayer,
			Alignment::Left, Colorf(0.0f, 0.0f, 0.0f, 0.3f * alpha), 1.0f, 0.0f, 0.0f, 0.0f);

		Colorf fontColor = Font::DefaultColor;
		fontColor.SetAlpha(alpha);
		_smallFont->DrawString(this, stringBuffer, charOffset, ViewSize.X * 0.5f, ViewSize.Y * 0.92f + offset, FontLayer,
			Alignment::Left, fontColor, 1.0f, 0.0f, 0.0f, 0.0f);

		if (_coinsTime > TotalTime) {
			_coinsTime = -1.0f;
		}
	}

	void HUD::DrawGems(int32_t& charOffset)
	{
		constexpr float StillTime = 120.0f;
		constexpr float TransitionTime = 60.0f;
		constexpr float TotalTime = StillTime + TransitionTime * 2.0f;

		if (_gemsTime < 0.0f) {
			return;
		}

		float offset, alpha;
		if (_gemsTime < TransitionTime) {
			offset = (TransitionTime - _gemsTime) / 10.0f;
			offset = -(offset * offset);
			alpha = std::max(_gemsTime / TransitionTime, 0.1f);
		} else if (_gemsTime > TransitionTime + StillTime) {
			offset = (_gemsTime - TransitionTime - StillTime) / 10.0f;
			offset = (offset * offset);
			alpha = (TotalTime - _gemsTime) / TransitionTime;
		} else {
			offset = 0.0f;
			alpha = 1.0f;
		}

		float animAlpha = alpha * alpha;
		DrawElement("PickupGem"_s, -1, ViewSize.X * 0.5f, ViewSize.Y * 0.92f + 2.5f + offset, ShadowLayer, Alignment::Right,
			Colorf(0.0f, 0.0f, 0.0f, 0.4f * animAlpha), 0.8f, 0.8f);
		DrawElement("PickupGem"_s, -1, ViewSize.X * 0.5f, ViewSize.Y * 0.92f + offset, MainLayer, Alignment::Right,
			Colorf(1.0f, 1.0f, 1.0f, 0.8f * animAlpha), 0.8f, 0.8f);

		char stringBuffer[32];
		snprintf(stringBuffer, countof(stringBuffer), "x%i", _gems);

		int32_t charOffsetShadow = charOffset;
		_smallFont->DrawString(this, stringBuffer, charOffsetShadow, ViewSize.X * 0.5f, ViewSize.Y * 0.92f + 2.5f + offset, FontShadowLayer,
			Alignment::Left, Colorf(0.0f, 0.0f, 0.0f, 0.3f * alpha), 1.0f, 0.0f, 0.0f, 0.0f);

		Colorf fontColor = Font::DefaultColor;
		fontColor.SetAlpha(alpha);
		_smallFont->DrawString(this, stringBuffer, charOffset, ViewSize.X * 0.5f, ViewSize.Y * 0.92f + offset, FontLayer,
			Alignment::Left, fontColor, 1.0f, 0.0f, 0.0f, 0.0f);

		if (_gemsTime > TotalTime) {
			_gemsTime = -1.0f;
		}
	}

	void HUD::DrawElement(const StringView& name, int32_t frame, float x, float y, uint16_t z, Alignment align, const Colorf& color, float scaleX, float scaleY, bool additiveBlending, float angle)
	{
		auto it = _graphics->find(String::nullTerminatedView(name));
		if (it == _graphics->end()) {
			return;
		}

		if (frame < 0) {
			frame = it->second.FrameOffset + ((int32_t)(AnimTime * it->second.FrameCount / it->second.AnimDuration) % it->second.FrameCount);
		}

		GenericGraphicResource* base = it->second.Base;
		Vector2f size = Vector2f(base->FrameDimensions.X * scaleX, base->FrameDimensions.Y * scaleY);
		Vector2f adjustedPos = ApplyAlignment(align, Vector2f(x - ViewSize.X * 0.5f, ViewSize.Y * 0.5f - y), size);

		Vector2i texSize = base->TextureDiffuse->size();
		int32_t col = frame % base->FrameConfiguration.X;
		int32_t row = frame / base->FrameConfiguration.X;
		Vector4f texCoords = Vector4f(
			float(base->FrameDimensions.X) / float(texSize.X),
			float(base->FrameDimensions.X * col) / float(texSize.X),
			float(base->FrameDimensions.Y) / float(texSize.Y),
			float(base->FrameDimensions.Y * row) / float(texSize.Y)
		);

		texCoords.W += texCoords.Z;
		texCoords.Z *= -1;

		DrawTexture(*base->TextureDiffuse.get(), adjustedPos, z, size, texCoords, color, additiveBlending, angle);
	}

	void HUD::DrawElementClipped(const StringView& name, int32_t frame, float x, float y, uint16_t z, Alignment align, const Colorf& color, float clipX, float clipY)
	{
		auto it = _graphics->find(String::nullTerminatedView(name));
		if (it == _graphics->end()) {
			return;
		}

		if (frame < 0) {
			frame = it->second.FrameOffset + ((int32_t)(AnimTime * it->second.FrameCount / it->second.AnimDuration) % it->second.FrameCount);
		}

		GenericGraphicResource* base = it->second.Base;
		Vector2f size = Vector2f(base->FrameDimensions.X * clipX, base->FrameDimensions.Y * clipY);
		Vector2f adjustedPos = ApplyAlignment(align, Vector2f(x - ViewSize.X * 0.5f - (1.0f - clipX) * 0.5f * base->FrameDimensions.X,
			ViewSize.Y * 0.5f - y - (1.0f - clipY) * 0.5f * base->FrameDimensions.Y), size);

		Vector2i texSize = base->TextureDiffuse->size();
		int32_t col = frame % base->FrameConfiguration.X;
		int32_t row = frame / base->FrameConfiguration.X;
		Vector4f texCoords = Vector4f(
			float(base->FrameDimensions.X) / float(texSize.X),
			float(base->FrameDimensions.X * col) / float(texSize.X),
			float(base->FrameDimensions.Y) / float(texSize.Y),
			float(base->FrameDimensions.Y * row) / float(texSize.Y)
		);

		texCoords.X *= clipX;
		texCoords.Z *= clipY;

		texCoords.W += texCoords.Z;
		texCoords.Z *= -1;

		DrawTexture(*base->TextureDiffuse.get(), adjustedPos, z, size, texCoords, color);
	}

	StringView HUD::GetCurrentWeapon(Actors::Player* player, WeaponType weapon, Vector2f& offset)
	{
		if (weapon == WeaponType::Toaster && player->_inWater) {
			offset.X += 1;
			offset.Y += 2;
			return "WeaponToasterDisabled"_s;
		} else if (weapon == WeaponType::Seeker) {
			offset.X += 2;
		} else if (weapon == WeaponType::TNT) {
			offset.X += 2;
		} else if (weapon == WeaponType::Electro) {
			offset.X += 6;
		}

		if ((player->_weaponUpgrades[(int32_t)weapon] & 0x01) != 0) {
			switch (weapon) {
				default:
				case WeaponType::Blaster:
					if (player->_playerType == PlayerType::Spaz) {
						return "WeaponPowerUpBlasterSpaz"_s;
					} else if (player->_playerType == PlayerType::Lori) {
						return "WeaponPowerUpBlasterLori"_s;
					} else {
						return "WeaponPowerUpBlasterJazz"_s;
					}

				case WeaponType::Bouncer: return "WeaponPowerUpBouncer"_s;
				case WeaponType::Freezer: return "WeaponPowerUpFreezer"_s;
				case WeaponType::Seeker: return "WeaponPowerUpSeeker"_s;
				case WeaponType::RF: return "WeaponPowerUpRF"_s;
				case WeaponType::Toaster: return "WeaponPowerUpToaster"_s;
				case WeaponType::TNT: return "WeaponPowerUpTNT"_s;
				case WeaponType::Pepper: return "WeaponPowerUpPepper"_s;
				case WeaponType::Electro: return "WeaponPowerUpElectro"_s;
				case WeaponType::Thunderbolt: return "WeaponPowerUpThunderbolt"_s;
			}
		} else {
			switch (weapon) {
				default:
				case WeaponType::Blaster:
					if (player->_playerType == PlayerType::Spaz) {
						return "WeaponBlasterSpaz"_s;
					} else if (player->_playerType == PlayerType::Lori) {
						return "WeaponBlasterLori"_s;
					} else {
						return "WeaponBlasterJazz"_s;
					}

				case WeaponType::Bouncer: return "WeaponBouncer"_s;
				case WeaponType::Freezer: return "WeaponFreezer"_s;
				case WeaponType::Seeker: return "WeaponSeeker"_s;
				case WeaponType::RF: return "WeaponRF"_s;
				case WeaponType::Toaster: return "WeaponToaster"_s;
				case WeaponType::TNT: return "WeaponTNT"_s;
				case WeaponType::Pepper: return "WeaponPepper"_s;
				case WeaponType::Electro: return "WeaponElectro"_s;
				case WeaponType::Thunderbolt: return "WeaponThunderbolt"_s;
			}
		}
	}

	void HUD::DrawWeaponWheel(Actors::Player* player)
	{
		if (_weaponWheelAnim <= 0.0f) {
			return;
		}

		auto it = _graphics->find(String::nullTerminatedView("WeaponWheel"_s));
		if (it == _graphics->end()) {
			return;
		}

		Texture& lineTexture = *it->second.Base->TextureDiffuse.get();

		if (!_levelHandler->_playerFrozenEnabled) {
			_levelHandler->_playerFrozenEnabled = true;
			_levelHandler->_playerFrozenMovement = _levelHandler->_playerRequiredMovement;
		}

		if (player->_weaponWheelState == Actors::Player::WeaponWheelState::Hidden && player->_sugarRushLeft <= 0.0f && _weaponWheelAnim >= WeaponWheelAnimDuration * 0.1f) {
			player->_weaponWheelState = Actors::Player::WeaponWheelState::Opening;
		}

		Vector2f center = Vector2f(ViewSize.X * 0.5f, ViewSize.Y * 0.5f);
		float angleStep = fTwoPi / _weaponWheelCount;

		float h = _levelHandler->_playerRequiredMovement.X;
		float v = _levelHandler->_playerRequiredMovement.Y;
		if (std::abs(h) + std::abs(v) < 0.5f) {
			h = 0.0f;
			v = 0.0f;
		}

		if (_weaponWheelVertices == nullptr) {
			_weaponWheelVertices = std::make_unique<Vertex[]>(WeaponWheelMaxVertices);
		}
		_weaponWheelVerticesCount = 0;
		_weaponWheelRenderCommandsCount = 0;

		float requestedAngle;
		int32_t requestedIndex;
		if (h == 0.0f && v == 0.0f) {
			requestedAngle = NAN;
			requestedIndex = -1;
		} else {
			requestedAngle = atan2f(v, h);
			if (requestedAngle < 0) {
				requestedAngle += fTwoPi;
			}

			float adjustedAngle = requestedAngle + fPiOver2 + angleStep * 0.5f;
			if (adjustedAngle >= fTwoPi) {
				adjustedAngle -= fTwoPi;
			}

			requestedIndex = (int32_t)(_weaponWheelCount * adjustedAngle / fTwoPi);
		}

		float alpha = _weaponWheelAnim / WeaponWheelAnimDuration;
		float easing = Menu::IMenuContainer::EaseOutCubic(alpha);
		float distance = 20 + (70 * easing);
		float distance2 = 10 + (50 * easing);
		float distance3 = distance2 * 2.0f;

		float alphaInner = std::min(Vector2f(h, v).Length() * easing * 1.5f - 0.6f, 1.0f);
		if (alphaInner > 0.0f) {
			DrawElement("WeaponWheelInner"_s, -1, center.X, center.Y, MainLayer + 5, Alignment::Center, Colorf(1.0f, 1.0f, 1.0f, alphaInner), easing, easing, true, -requestedAngle);
		}

		float angle = -fPiOver2;
		for (int32_t i = 0, j = 0; i < countof(player->_weaponAmmo); i++) {
			if (player->_weaponAmmo[i] != 0) {
				float x = cosf(angle) * distance;
				float y = sinf(angle) * distance;

				Vector2f pos = Vector2f(center.X + x, center.Y + y);
				StringView weapon = GetCurrentWeapon(player, (WeaponType)i, pos);
				Colorf color2;
				float scale;
				bool isSelected = (j == requestedIndex);
				if (isSelected) {
					_lastWeaponWheelIndex = i;
					color2 = Colorf(1.0f, 0.8f, 0.5f, alpha);
					scale = 1.0f;
				} else {
					color2 = Colorf(1.0f, 1.0f, 1.0f, alpha * 0.7f);
					scale = 0.9f;
				}

				DrawElement("WeaponWheelDim"_s, -1, pos.X, pos.Y, ShadowLayer - 10, Alignment::Center, Colorf(0.0f, 0.0f, 0.0f, alpha * 0.6f), 5.0f, 5.0f);
				DrawElement(weapon, -1, pos.X, pos.Y, MainLayer + 10, Alignment::Center, Colorf(1.0f, 1.0f, 1.0f, isSelected ? alpha : alpha * 0.7f), scale, scale);

				if (PreferencesCache::WeaponWheel == WeaponWheelStyle::EnabledWithAmmoCount) {
					char stringBuffer[32];
					StringView ammoCount;
					if (player->_weaponAmmo[i] == UINT16_MAX) {
						ammoCount = "x\u221E"_s;
					} else {
						stringBuffer[0] = 'x';
						i32tos(player->_weaponAmmo[i] / 256, stringBuffer + 1);
						ammoCount = stringBuffer;
					}

					int32_t charOffset = 0;
					_smallFont->DrawString(this, ammoCount, charOffset, center.X + cosf(angle) * distance * 1.4f, center.Y + sinf(angle) * distance * 1.4f, FontLayer,
						Alignment::Center, isSelected ? Colorf(0.62f, 0.44f, 0.34f, 0.5f * alpha) : Colorf(0.45f, 0.45f, 0.45f, 0.48f * alpha), 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f);
				}

				float angle2 = fTwoPi - angle;
				float angleFrom = angle2 - angleStep * 0.4f;
				float angleTo = angle2 + angleStep * 0.4f;

				Colorf color1 = Colorf(0.0f, 0.0f, 0.0f, alpha * 0.3f);
				DrawWeaponWheelSegment(center.X - distance2 - 1, center.Y - distance2 - 1, distance3, distance3, ShadowLayer, angleFrom, angleTo, lineTexture, color1);
				DrawWeaponWheelSegment(center.X - distance2 - 1, center.Y - distance2 + 1, distance3, distance3, ShadowLayer, angleFrom, angleTo, lineTexture, color1);
				DrawWeaponWheelSegment(center.X - distance2 + 1, center.Y - distance2 - 1, distance3, distance3, ShadowLayer, angleFrom, angleTo, lineTexture, color1);
				DrawWeaponWheelSegment(center.X - distance2 + 1, center.Y - distance2 + 1, distance3, distance3, ShadowLayer, angleFrom, angleTo, lineTexture, color1);

				DrawWeaponWheelSegment(center.X - distance2, center.Y - distance2, distance3, distance3, MainLayer, angleFrom, angleTo, lineTexture, color2);
				if (isSelected) {
					DrawWeaponWheelSegment(center.X - distance2 - 1.0f, center.Y - distance2 - 1.0f, distance3 + 2.0f, distance3 + 2.0f, MainLayer + 1, angleFrom + fRadAngle1, angleTo - fRadAngle1, lineTexture, Colorf(1.0f, 0.8f, 0.5f, alpha * 0.3f));
				}

				angle += angleStep;
				j++;
			}
		}
	}

	bool HUD::PrepareWeaponWheel(Actors::Player* player, int& weaponCount)
	{
		weaponCount = 0;

		if (PreferencesCache::WeaponWheel == WeaponWheelStyle::Disabled || player == nullptr || !player->_controllable || !player->_controllableExternal || player->_playerType == PlayerType::Frog) {
			if (_weaponWheelAnim > 0.0f) {
				_weaponWheelShown = false;
				_lastWeaponWheelIndex = -1;
			}
			return false;
		}

		bool isGamepad;
		if (!_levelHandler->PlayerActionPressed(player->_playerIndex, PlayerActions::ChangeWeapon, true, isGamepad) || !isGamepad) {
			if (_weaponWheelAnim > 0.0f) {
				if (_weaponWheelAnim < WeaponWheelAnimDuration * 0.5f) {
					// Switch to the next weapon on short press
					if (_weaponWheelShown) {
						player->SwitchToNextWeapon();
					}
				} else if (_lastWeaponWheelIndex != -1) {
					player->SwitchToWeaponByIndex((uint32_t)_lastWeaponWheelIndex);
				}
				_weaponWheelShown = false;
				_lastWeaponWheelIndex = -1;
				weaponCount = GetWeaponCount(player);
			}
			return false;
		}

		_weaponWheelShown = true;
		weaponCount = GetWeaponCount(player);
		return (weaponCount > 0);
	}

	int32_t HUD::GetWeaponCount(Actors::Player* player)
	{
		int32_t weaponCount = 0;

		for (int32_t i = 0; i < countof(player->_weaponAmmo); i++) {
			if (player->_weaponAmmo[i] != 0) {
				weaponCount++;
			}
		}

		// Player must have at least 2 weapons
		if (weaponCount < 2) {
			weaponCount = 0;
		}

		return weaponCount;
	}

	void HUD::DrawWeaponWheelSegment(float x, float y, float width, float height, uint16_t z, float minAngle, float maxAngle, const Texture& texture, const Colorf& color)
	{
		width *= 0.5f; x += width;
		height *= 0.5f; y += height;

		x -= ViewSize.X * 0.5f;
		y -= ViewSize.Y * 0.5f;
		y = -y;

		float angleRange = std::min(maxAngle - minAngle, fRadAngle360);
		int32_t segmentNum = std::clamp((int32_t)std::round(powf(std::max(width, height), 0.65f) * 3.5f * angleRange / fRadAngle360), 4, 128);
		float angleStep = angleRange / (segmentNum - 1);
		int32_t vertexCount = segmentNum + 2;
		float angle = minAngle;

		Vertex* vertices = &_weaponWheelVertices[_weaponWheelVerticesCount];
		_weaponWheelVerticesCount += vertexCount;

		if (_weaponWheelVerticesCount > WeaponWheelMaxVertices) {
			// This shouldn't happen, 512 vertices should be enough
			return;
		}

		constexpr float Mult = 2.2f;

		{
			int32_t j = 0;
			vertices[j].X = x + cosf(angle) * (width * Mult - 0.5f);
			vertices[j].Y = y + sinf(angle) * (height * Mult - 0.5f);
			vertices[j].U = 0.0f;
			vertices[j].V = 0.0f;
		}

		for (int32_t i = 1; i < vertexCount - 1; i++) {
			vertices[i].X = x + cosf(angle) * (width - 0.5f);
			vertices[i].Y = y + sinf(angle) * (height - 0.5f);
			vertices[i].U = 0.15f + (0.7f * (float)(i - 1) / (vertexCount - 3));
			vertices[i].V = 0.0f;

			angle += angleStep;
		}

		{
			angle -= angleStep;

			int32_t j = vertexCount - 1;
			vertices[j].X = x + cosf(angle) * (width * Mult - 0.5f);
			vertices[j].Y = y + sinf(angle) * (height * Mult - 0.5f);
			vertices[j].U = 1.0f;
			vertices[j].V = 0.0f;
		}

		// Create render command
		RenderCommand* command;
		if (_weaponWheelRenderCommandsCount < _weaponWheelRenderCommands.size()) {
			command = _weaponWheelRenderCommands[_weaponWheelRenderCommandsCount].get();
			_weaponWheelRenderCommandsCount++;
		} else {
			command = _weaponWheelRenderCommands.emplace_back(std::make_unique<RenderCommand>()).get();
			command->material().setBlendingEnabled(true);
		}

		if (command->material().setShaderProgramType(Material::ShaderProgramType::MESH_SPRITE)) {
			command->material().reserveUniformsDataMemory();

			GLUniformCache* textureUniform = command->material().uniform(Material::TextureUniformName);
			if (textureUniform && textureUniform->intValue(0) != 0) {
				textureUniform->setIntValue(0); // GL_TEXTURE0
			}
		}

		command->geometry().setDrawParameters(GL_LINE_STRIP, 0, vertexCount);
		command->geometry().setNumElementsPerVertex(VertexFloats);
		command->geometry().setHostVertexPointer((const float*)vertices);

		command->material().setBlendingFactors(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto instanceBlock = command->material().uniformBlock(Material::InstanceBlockName);
		instanceBlock->uniform(Material::TexRectUniformName)->setFloatValue(1.0f, 0.0f, 1.0f, 0.0f);
		instanceBlock->uniform(Material::SpriteSizeUniformName)->setFloatValue(1.0f, 1.0f);
		instanceBlock->uniform(Material::ColorUniformName)->setFloatVector(color.Data());

		command->setTransformation(Matrix4x4f::Identity);
		command->setLayer(z);
		command->material().setTexture(texture);

		DrawRenderCommand(command);
	}

	HUD::TouchButtonInfo HUD::CreateTouchButton(PlayerActions action, const StringView& identifier, Alignment align, float x, float y, float w, float h)
	{
		TouchButtonInfo info;
		info.Action = action;
		info.Left = x * LevelHandler::DefaultWidth * 0.5f;
		info.Top = y * LevelHandler::DefaultWidth * 0.5f;
		info.Width = w * LevelHandler::DefaultWidth * 0.5f;
		info.Height = h * LevelHandler::DefaultWidth * 0.5f;

		if (!identifier.empty()) {
			auto it = _graphics->find(String::nullTerminatedView(identifier));
			info.Graphics = (it != _graphics->end() ? &it->second : nullptr);
		} else {
			info.Graphics = nullptr;
		}

		info.CurrentPointerId = -1;
		info.Align = align;
		return info;
	}

	bool HUD::IsOnButton(const HUD::TouchButtonInfo& button, float x, float y)
	{
		if ((button.Align & Fixed) != Fixed) {
			if ((button.Align & Alignment::Right) == Alignment::Right) {
				x += PreferencesCache::TouchRightPadding.X;
				y -= PreferencesCache::TouchRightPadding.Y;
			} else {
				x -= PreferencesCache::TouchLeftPadding.X;
				y -= PreferencesCache::TouchLeftPadding.Y;
			}
		}

		float left = button.Left;
		if ((button.Align & Alignment::Right) == Alignment::Right) { left = ViewSize.X - button.Width - left; }
		if (x < left) return false;

		float top = button.Top;
		if ((button.Align & Alignment::Bottom) == Alignment::Bottom) { top = ViewSize.Y - button.Height - top; }
		if (y < top) return false;

		float right = left + button.Width;
		if (x > right) return false;

		float bottom = top + button.Height;
		if (y > bottom) return false;

		return true;
	}

	void HUD::UpdateRgbLights(float timeMult, Actors::Player* player)
	{
#if !defined(DEATH_TARGET_ANDROID) && !defined(DEATH_TARGET_IOS) && !defined(DEATH_TARGET_WINDOWS_RT)
		if (!PreferencesCache::EnableRgbLights) {
			_rgbHealthLast = 0.0f;
			return;
		}

		RgbLights& rgbLights = RgbLights::Get();
		if (!rgbLights.IsSupported()) {
			return;
		}

		_rgbLightsTime -= timeMult;
		if (_rgbLightsTime > 0.0f) {
			return;
		}

		_rgbLightsTime += RgbLights::RefreshRate;

		float health = std::clamp((float)player->_health / player->_maxHealth, 0.0f, 1.0f);
		if (std::abs(health - _rgbHealthLast) < 0.001f && _rgbAmbientLight == _levelHandler->_ambientColor.W) {
			return;
		}

		_rgbHealthLast = lerp(_rgbHealthLast, health, 0.2f);
		_rgbAmbientLight = _levelHandler->_ambientColor.W;

		constexpr int32_t KeyMax2 = 14;
		Color colors[RgbLights::ColorsSize] { };

		Color* captionTile = _levelHandler->_tileMap->GetCaptionTile();
		if (captionTile != nullptr) {
			uint8_t colorMultiplier = (uint8_t)(0.2f * 255.0f * _rgbAmbientLight);
			for (int32_t i = 0; i < countof(KeyLayout); i++) {
				int32_t x = KeyLayout[i] % AURA_KEYBOARD_WIDTH;
				int32_t y = KeyLayout[i] / AURA_KEYBOARD_WIDTH;
				Color tileColor = captionTile[y * 32 + x];
				colors[AURA_COLORS_LIMITED_SIZE + i] = Color(tileColor.R() * colorMultiplier / 255, tileColor.G() * colorMultiplier / 255, tileColor.B() * colorMultiplier / 255);
			}
		}

		int32_t percent, percentR, percentG;
		percent = (int32_t)(_rgbHealthLast * 255);
		percentG = percent * percent / 255;
		percentR = (255 - (percent - 120) * 2);
		percentR = std::clamp(percentR, 0, 255);

		for (int32_t i = 0; i < KeyMax2; i++) {
			int32_t intensity = (int32_t)((_rgbHealthLast - ((float)i / KeyMax2)) * 255 * KeyMax2);
			intensity = std::clamp(intensity, 0, 200);

			if (intensity > 0) {
				colors[(int32_t)AuraLight::Tilde + i] = Color(percentR * intensity / 255, percentG * intensity / 255, 0);
				colors[(int32_t)AuraLight::Tab + i] = Color(percentR * intensity / (255 * 12), percentG * intensity / (255 * 12), 0);
			}
		}

		auto mapings = ControlScheme::GetMappings();
		AuraLight l = KeyToAuraLight(mapings[(int32_t)PlayerActions::Up].Key1);
		if (l != AuraLight::Unknown) colors[(int32_t)l] = Color(100, 100, 100);
		l = KeyToAuraLight(mapings[(int32_t)PlayerActions::Down].Key1);
		if (l != AuraLight::Unknown) colors[(int32_t)l] = Color(100, 100, 100);
		l = KeyToAuraLight(mapings[(int32_t)PlayerActions::Left].Key1);
		if (l != AuraLight::Unknown) colors[(int32_t)l] = Color(100, 100, 100);
		l = KeyToAuraLight(mapings[(int32_t)PlayerActions::Right].Key1);
		if (l != AuraLight::Unknown) colors[(int32_t)l] = Color(100, 100, 100);

		l = KeyToAuraLight(mapings[(int32_t)PlayerActions::Fire].Key1);
		if (l != AuraLight::Unknown) colors[(int32_t)l] = Color(160, 10, 10);
		l = KeyToAuraLight(mapings[(int32_t)PlayerActions::Jump].Key1);
		if (l != AuraLight::Unknown) colors[(int32_t)l] = Color(10, 80, 160);
		l = KeyToAuraLight(mapings[(int32_t)PlayerActions::Run].Key1);
		if (l != AuraLight::Unknown) colors[(int32_t)l] = Color(10, 170, 10);
		l = KeyToAuraLight(mapings[(int32_t)PlayerActions::ChangeWeapon].Key1);
		if (l != AuraLight::Unknown) colors[(int32_t)l] = Color(150, 140, 10);

		rgbLights.Update(colors);
#endif
	}

	AuraLight HUD::KeyToAuraLight(KeySym key)
	{
		switch (key) {
			case KeySym::BACKSPACE: return AuraLight::Backspace;
			case KeySym::TAB: return AuraLight::Tab;
			case KeySym::RETURN: return AuraLight::Enter;
			case KeySym::ESCAPE: return AuraLight::Esc;
			case KeySym::SPACE: return AuraLight::Space;
			//case KeySym::QUOTE: return AuraLight::Quote;
			//case KeySym::PLUS: return AuraLight:: + ;
			case KeySym::COMMA: return AuraLight::Comma;
			case KeySym::MINUS: return AuraLight::Minus;
			case KeySym::PERIOD: return AuraLight::Period;
			case KeySym::SLASH: return AuraLight::Slash;
			case KeySym::N0: return AuraLight::Zero;
			case KeySym::N1: return AuraLight::One;
			case KeySym::N2: return AuraLight::Two;
			case KeySym::N3: return AuraLight::Three;
			case KeySym::N4: return AuraLight::Four;
			case KeySym::N5: return AuraLight::Five;
			case KeySym::N6: return AuraLight::Six;
			case KeySym::N7: return AuraLight::Seven;
			case KeySym::N8: return AuraLight::Eight;
			case KeySym::N9: return AuraLight::Nine;
			case KeySym::SEMICOLON: return AuraLight::Semicolon;
			case KeySym::LEFTBRACKET: return AuraLight::OpenBracket;
			case KeySym::BACKSLASH: return AuraLight::Backslash;
			case KeySym::RIGHTBRACKET: return AuraLight::CloseBracket;
			//case KeySym::BACKQUOTE: return AuraLight::Backquote;

			case KeySym::A: return AuraLight::A;
			case KeySym::B: return AuraLight::B;
			case KeySym::C: return AuraLight::C;
			case KeySym::D: return AuraLight::D;
			case KeySym::E: return AuraLight::E;
			case KeySym::F: return AuraLight::F;
			case KeySym::G: return AuraLight::G;
			case KeySym::H: return AuraLight::H;
			case KeySym::I: return AuraLight::I;
			case KeySym::J: return AuraLight::J;
			case KeySym::K: return AuraLight::K;
			case KeySym::L: return AuraLight::L;
			case KeySym::M: return AuraLight::M;
			case KeySym::N: return AuraLight::N;
			case KeySym::O: return AuraLight::O;
			case KeySym::P: return AuraLight::P;
			case KeySym::Q: return AuraLight::Q;
			case KeySym::R: return AuraLight::R;
			case KeySym::S: return AuraLight::S;
			case KeySym::T: return AuraLight::T;
			case KeySym::U: return AuraLight::U;
			case KeySym::V: return AuraLight::V;
			case KeySym::W: return AuraLight::W;
			case KeySym::X: return AuraLight::X;
			case KeySym::Y: return AuraLight::Y;
			case KeySym::Z: return AuraLight::Z;
			case KeySym::Delete: return AuraLight::Delete;

			case KeySym::KP0: return AuraLight::NumZero;
			case KeySym::KP1: return AuraLight::NumOne;
			case KeySym::KP2: return AuraLight::NumTwo;
			case KeySym::KP3: return AuraLight::NumThree;
			case KeySym::KP4: return AuraLight::NumFour;
			case KeySym::KP5: return AuraLight::NumFive;
			case KeySym::KP6: return AuraLight::NumSix;
			case KeySym::KP7: return AuraLight::NumSeven;
			case KeySym::KP8: return AuraLight::NumEight;
			case KeySym::KP9: return AuraLight::NumNine;
			case KeySym::KP_PERIOD: return AuraLight::NumPeriod;
			case KeySym::KP_DIVIDE: return AuraLight::NumSlash;
			case KeySym::KP_MULTIPLY: return AuraLight::NumAsterisk;
			case KeySym::KP_MINUS: return AuraLight::NumMinus;
			case KeySym::KP_PLUS: return AuraLight::NumPlus;
			case KeySym::KP_ENTER: return AuraLight::NumEnter;
			case KeySym::KP_EQUALS: return AuraLight::NumEnter;

			case KeySym::UP: return AuraLight::ArrowUp;
			case KeySym::DOWN: return AuraLight::ArrowDown;
			case KeySym::RIGHT: return AuraLight::ArrowRight;
			case KeySym::LEFT: return AuraLight::ArrowLeft;
			case KeySym::INSERT: return AuraLight::Insert;
			case KeySym::HOME: return AuraLight::Home;
			case KeySym::END: return AuraLight::End;
			case KeySym::PAGEUP: return AuraLight::PageUp;
			case KeySym::PAGEDOWN: return AuraLight::PageDown;

			case KeySym::F1: return AuraLight::F1;
			case KeySym::F2: return AuraLight::F2;
			case KeySym::F3: return AuraLight::F3;
			case KeySym::F4: return AuraLight::F4;
			case KeySym::F5: return AuraLight::F5;
			case KeySym::F6: return AuraLight::F6;
			case KeySym::F7: return AuraLight::F7;
			case KeySym::F8: return AuraLight::F8;
			case KeySym::F9: return AuraLight::F9;
			case KeySym::F10: return AuraLight::F10;
			case KeySym::F11: return AuraLight::F11;
			case KeySym::F12: return AuraLight::F12;

			case KeySym::NUM_LOCK: return AuraLight::NumLock;
			case KeySym::CAPS_LOCK: return AuraLight::CapsLock;
			case KeySym::SCROLL_LOCK: return AuraLight::ScrollLock;
			case KeySym::RSHIFT: return AuraLight::RightShift;
			case KeySym::LSHIFT: return AuraLight::LeftShift;
			case KeySym::RCTRL: return AuraLight::RightCtrl;
			case KeySym::LCTRL: return AuraLight::LeftCtrl;
			case KeySym::RALT: return AuraLight::RightAlt;
			case KeySym::LALT: return AuraLight::LeftAlt;
			case KeySym::PAUSE: return AuraLight::PauseBreak;
			case KeySym::MENU: return AuraLight::Menu;

			default: return AuraLight::Unknown;
		}
	}
}