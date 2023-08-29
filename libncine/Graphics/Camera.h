#pragma once

#include "../Primitives/Matrix4x4.h"
#include "../Primitives/Rect.h"

namespace nCine {

	/// The camera class that handles matrices for shaders
	class Camera
	{
	public:
		struct ProjectionValues
		{
			float left;
			float right;
			float top;
			float bottom;
			float near;
			float far;

			ProjectionValues()
				: left(0.0f), right(0.0f), top(0.0f), bottom(0.0f), near(-1.0f), far(1.0f) {}

			ProjectionValues(float ll, float rr, float tt, float bb)
				: left(ll), right(rr), top(tt), bottom(bb), near(-1.0f), far(1.0f) {}
		};

		struct ViewValues
		{
			Vector2f position;
			float rotation;
			float scale;

			ViewValues()
				: position(0.0f, 0.0f), rotation(0.0f), scale(1.0f) {}

			ViewValues(float xx, float yy, float rr, float ss)
				: position(xx, yy), rotation(rr), scale(ss) {}
		};

		/// Creates a camera with default matrices
		Camera();

		inline const ProjectionValues& projectionValues() const {
			return projectionValues_;
		}
		inline const ViewValues& viewValues() const {
			return viewValues_;
		}

		inline const Matrix4x4f& projection() const {
			return projection_;
		}
		inline const Matrix4x4f& view() const {
			return view_;
		}

		void setOrthoProjection(float left, float right, float top, float bottom);
		void setOrthoProjection(const ProjectionValues& values);

		void setView(const Vector2f& pos, float rotation, float scale);
		void setView(float x, float y, float rotation, float scale);
		void setView(const ViewValues& values);

		inline unsigned long int updateFrameProjectionMatrix() const {
			return updateFrameProjectionMatrix_;
		}
		inline unsigned long int updateFrameViewMatrix() const {
			return updateFrameViewMatrix_;
		}

	private:
		ProjectionValues projectionValues_;
		ViewValues viewValues_;
		Matrix4x4f projection_;
		Matrix4x4f view_;
		/// Last frame when the projection matrix was changed
		unsigned long int updateFrameProjectionMatrix_;
		/// Last frame when the model matrix was changed
		unsigned long int updateFrameViewMatrix_;
	};

}
