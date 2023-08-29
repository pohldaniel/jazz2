﻿#pragma once

#include "ILevelHandler.h"
#include "IStateHandler.h"
#include "IRootController.h"
#include "WeatherType.h"
#include "Events/EventMap.h"
#include "Events/EventSpawner.h"
#include "Tiles/TileMap.h"
#include "Collisions/DynamicTreeBroadPhase.h"
#include "UI/UpscaleRenderPass.h"
#include "UI/Menu/InGameMenu.h"

#include "Graphics/Shader.h"
#include "Audio/AudioBufferPlayer.h"
#include "Audio/AudioStreamPlayer.h"

namespace Jazz2
{
	namespace Actors
	{
		class Player;
	}

	namespace Actors::Bosses
	{
		class BossBase;
	}

#if defined(WITH_ANGELSCRIPT)
	namespace Scripting
	{
		class LevelScriptLoader;
	}
#endif

	namespace UI
	{
		class HUD;
	}

	namespace UI::Menu
	{
		class InGameMenu;
	}

	class LevelHandler : public ILevelHandler, public IStateHandler
	{
		friend class ContentResolver;
#if defined(WITH_ANGELSCRIPT)
		friend class Scripting::LevelScriptLoader;
#endif
		friend class UI::HUD;
		friend class UI::Menu::InGameMenu;

	public:
		static constexpr int32_t DefaultWidth = 720;
		static constexpr int32_t DefaultHeight = 405;
		static constexpr int32_t ActivateTileRange = 26;

		LevelHandler(IRootController* root, const LevelInitialization& levelInit);
		~LevelHandler() override;

		bool IsLoaded() const {
			return (_tileMap != nullptr && _eventMap != nullptr);
		}

		Events::EventSpawner* EventSpawner() override {
			return &_eventSpawner;
		}
		Events::EventMap* EventMap() override {
			return _eventMap.get();
		}
		Tiles::TileMap* TileMap() override {
			return _tileMap.get();
		}

		GameDifficulty Difficulty() const override {
			return _difficulty;
		}

		bool IsReforged() const override {
			return _isReforged;
		}

		Recti LevelBounds() const override;

		float ElapsedFrames() const override {
			return _elapsedFrames;
		}

		float WaterLevel() const override;

		const SmallVectorImpl<std::shared_ptr<Actors::ActorBase>>& GetActors() const override;
		const SmallVectorImpl<Actors::Player*>& GetPlayers() const override;

		float GetAmbientLight() const override;
		void SetAmbientLight(float value) override;

		void OnBeginFrame() override;
		void OnEndFrame() override;
		void OnInitializeViewport(int32_t width, int32_t height) override;

		void OnKeyPressed(const KeyboardEvent& event) override;
		void OnKeyReleased(const KeyboardEvent& event) override;
		void OnTouchEvent(const TouchEvent& event) override;

		void AddActor(std::shared_ptr<Actors::ActorBase> actor) override;

		std::shared_ptr<AudioBufferPlayer> PlaySfx(AudioBuffer* buffer, const Vector3f& pos, bool sourceRelative, float gain = 1.0f, float pitch = 1.0f) override;
		std::shared_ptr<AudioBufferPlayer> PlayCommonSfx(const StringView& identifier, const Vector3f& pos, float gain = 1.0f, float pitch = 1.0f) override;
		void WarpCameraToTarget(const std::shared_ptr<Actors::ActorBase>& actor, bool fast = false) override;
		bool IsPositionEmpty(Actors::ActorBase* self, const AABBf& aabb, TileCollisionParams& params, Actors::ActorBase** collider) override;
		void FindCollisionActorsByAABB(Actors::ActorBase* self, const AABBf& aabb, const std::function<bool(Actors::ActorBase*)>& callback) override;
		void FindCollisionActorsByRadius(float x, float y, float radius, const std::function<bool(Actors::ActorBase*)>& callback) override;
		void GetCollidingPlayers(const AABBf& aabb, const std::function<bool(Actors::ActorBase*)> callback) override;

		void BroadcastTriggeredEvent(Actors::ActorBase* initiator, EventType eventType, uint8_t* eventParams) override;
		void BeginLevelChange(ExitType exitType, const StringView& nextLevel) override;
		void HandleGameOver() override;
		bool HandlePlayerDied(const std::shared_ptr<Actors::ActorBase>& player) override;
		void SetCheckpoint(Vector2f pos) override;
		void RollbackToCheckpoint() override;
		void ActivateSugarRush() override;
		void ShowLevelText(const StringView& text) override;
		void ShowCoins(int32_t count) override;
		void ShowGems(int32_t count) override;
		StringView GetLevelText(uint32_t textId, int32_t index = -1, uint32_t delimiter = 0) override;
		void OverrideLevelText(uint32_t textId, const StringView& value) override;
		void LimitCameraView(int left, int width) override;
		void ShakeCameraView(float duration) override;
		void SetWeather(WeatherType type, uint8_t intensity) override;
		bool BeginPlayMusic(const StringView& path, bool setDefault = false, bool forceReload = false) override;

		bool PlayerActionPressed(int32_t index, PlayerActions action, bool includeGamepads = true) override;
		bool PlayerActionPressed(int32_t index, PlayerActions action, bool includeGamepads, bool& isGamepad) override;
		bool PlayerActionHit(int32_t index, PlayerActions action, bool includeGamepads = true) override;
		bool PlayerActionHit(int32_t index, PlayerActions action, bool includeGamepads, bool& isGamepad) override;
		float PlayerHorizontalMovement(int32_t index) override;
		float PlayerVerticalMovement(int32_t index) override;

		Vector2f GetCameraPos() {
			return _cameraPos;
		}

		Vector2i GetViewSize() {
			return _view->size();
		}

	private:
		IRootController* _root;

		class LightingRenderer : public SceneNode
		{
		public:
			LightingRenderer(LevelHandler* owner)
				: _owner(owner), _renderCommandsCount(0)
			{
				_emittedLightsCache.reserve(32);
				setVisitOrderState(SceneNode::VisitOrderState::Disabled);
			}

			bool OnDraw(RenderQueue& renderQueue) override;

		private:
			LevelHandler* _owner;
			SmallVector<std::unique_ptr<RenderCommand>, 0> _renderCommands;
			int32_t _renderCommandsCount;
			SmallVector<LightEmitter, 0> _emittedLightsCache;

			RenderCommand* RentRenderCommand();
		};

		class BlurRenderPass : public SceneNode
		{
		public:
			BlurRenderPass(LevelHandler* owner)
				: _owner(owner)
			{
				setVisitOrderState(SceneNode::VisitOrderState::Disabled);
			}

			void Initialize(Texture* source, int32_t width, int32_t height, const Vector2f& direction);
			void Register();

			bool OnDraw(RenderQueue& renderQueue) override;

			Texture* GetTarget() const {
				return _target.get();
			}

		private:
			LevelHandler* _owner;
			std::unique_ptr<Texture> _target;
			std::unique_ptr<Viewport> _view;
			std::unique_ptr<Camera> _camera;
			RenderCommand _renderCommand;

			Texture* _source;
			bool _downsampleOnly;
			Vector2f _direction;
		};

		class CombineRenderer : public SceneNode
		{
		public:
			CombineRenderer(LevelHandler* owner)
				: _owner(owner)
			{
				setVisitOrderState(SceneNode::VisitOrderState::Disabled);
			}

			void Initialize(int32_t width, int32_t height);

			bool OnDraw(RenderQueue& renderQueue) override;

		private:
			LevelHandler* _owner;
			RenderCommand _renderCommand;
			RenderCommand _renderCommandWithWater;
			Vector2f _size;
		};

		std::unique_ptr<LightingRenderer> _lightingRenderer;
		std::unique_ptr<CombineRenderer> _combineRenderer;
		std::unique_ptr<Viewport> _lightingView;
		std::unique_ptr<Texture> _lightingBuffer;

		Shader* _lightingShader;
		Shader* _blurShader;
		Shader* _downsampleShader;
		Shader* _combineShader;
		Shader* _combineWithWaterShader;

		BlurRenderPass _downsamplePass;
		BlurRenderPass _blurPass2;
		BlurRenderPass _blurPass1;
		BlurRenderPass _blurPass3;
		BlurRenderPass _blurPass4;
		UI::UpscaleRenderPassWithClipping _upscalePass;

		std::unique_ptr<SceneNode> _rootNode;
		std::unique_ptr<Viewport> _view;
		std::unique_ptr<Texture> _viewTexture;
		std::unique_ptr<Camera> _camera;
		std::unique_ptr<Texture> _noiseTexture;

#if defined(WITH_ANGELSCRIPT)
		std::unique_ptr<Scripting::LevelScriptLoader> _scripts;
#endif
		SmallVector<std::shared_ptr<Actors::ActorBase>, 0> _actors;
		SmallVector<Actors::Player*, LevelInitialization::MaxPlayerCount> _players;

		String _levelFileName;
		String _episodeName;
		String _defaultNextLevel;
		String _defaultSecretLevel;
		GameDifficulty _difficulty;
		String _musicDefaultPath, _musicCurrentPath;
		Recti _levelBounds;
		bool _isReforged, _cheatsUsed;
		char _cheatsBuffer[9];
		uint32_t _cheatsBufferLength;
		SmallVector<String, 0> _levelTexts;

		String _nextLevel;
		ExitType _nextLevelType;
		float _nextLevelTime;

		Events::EventSpawner _eventSpawner;
		std::unique_ptr<Events::EventMap> _eventMap;
		std::unique_ptr<Tiles::TileMap> _tileMap;
		Collisions::DynamicTreeBroadPhase _collisions;

		float _elapsedFrames;
		float _checkpointFrames;
		Rectf _viewBounds;
		Rectf _viewBoundsTarget;
		Vector2f _cameraPos;
		Vector2f _cameraLastPos;
		Vector2f _cameraDistanceFactor;
		float _shakeDuration;
		Vector2f _shakeOffset;
		float _waterLevel;
		float _ambientLightTarget;
		Vector4f _ambientColor;
		std::unique_ptr<AudioStreamPlayer> _music;
		SmallVector<std::shared_ptr<AudioBufferPlayer>> _playingSounds;
		Metadata* _commonResources;
		std::unique_ptr<UI::HUD> _hud;
		std::shared_ptr<UI::Menu::InGameMenu> _pauseMenu;
		std::shared_ptr<AudioBufferPlayer> _sugarRushMusic;
		std::shared_ptr<Actors::Bosses::BossBase> _activeBoss;
		WeatherType _weatherType;
		uint8_t _weatherIntensity;

		BitArray _pressedKeys;
		uint64_t _pressedActions;
		uint32_t _overrideActions;
		Vector2f _playerRequiredMovement;
		Vector2f _playerFrozenMovement;
		bool _playerFrozenEnabled;
		uint32_t _lastPressedNumericKey;

		void OnLevelLoaded(const StringView& fullPath, const StringView& name, const StringView& nextLevel, const StringView& secretLevel,
			std::unique_ptr<Tiles::TileMap>& tileMap, std::unique_ptr<Events::EventMap>& eventMap,
			const StringView& musicPath, const Vector4f& ambientColor, WeatherType weatherType, uint8_t weatherIntensity, uint16_t waterLevel, SmallVectorImpl<String>& levelTexts);

		void ResolveCollisions(float timeMult);
		void InitializeCamera();
		void UpdateCamera(float timeMult);
		void UpdatePressedActions();

		void PauseGame();
		void ResumeGame();
	};
}