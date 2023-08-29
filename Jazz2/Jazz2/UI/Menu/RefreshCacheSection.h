﻿#pragma once

#include <CommonBase.h>

#if !defined(DEATH_TARGET_EMSCRIPTEN)

#include "MenuSection.h"

#include "Threading/Thread.h"

namespace Jazz2::UI::Menu
{
	class RefreshCacheSection : public MenuSection
	{
	public:
		RefreshCacheSection();

		void OnShow(IMenuContainer* root) override;
		void OnUpdate(float timeMult) override;
		void OnDraw(Canvas* canvas) override;
		void OnTouchEvent(const nCine::TouchEvent& event, const Vector2i& viewSize) override;

	private:
		float _animation;
		bool _done;
#if defined(WITH_THREADS)
		Thread _thread;
#endif
	};
}

#endif