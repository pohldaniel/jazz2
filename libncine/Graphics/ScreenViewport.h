#pragma once

#include "Viewport.h"

namespace nCine
{
	/// The class handling the screen viewport
	class ScreenViewport : public Viewport
	{
	public:
		/// Creates the screen viewport
		ScreenViewport();

		/// Changes the size, viewport rectangle and projection matrix of the screen viewport
		void resize(int width, int height);

	private:
		void update();
		void visit();
		void sortAndCommitQueue();
		void draw();

		/// Deleted copy constructor
		ScreenViewport(const ScreenViewport&) = delete;
		/// Deleted assignment operator
		ScreenViewport& operator=(const ScreenViewport&) = delete;

		friend class Application;
	};
}
