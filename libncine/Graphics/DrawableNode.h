#pragma once

#include "SceneNode.h"
#include "../Primitives/Rect.h"
#include "RenderCommand.h"

namespace nCine
{
	class RenderCommand;
	class RenderQueue;

	/// A class for objects that can be drawn through the render queue
	class DrawableNode : public SceneNode
	{
	public:
		// Notable anchor points for a drawable node
		static const Vector2f AnchorCenter;
		static const Vector2f AnchorBottomLeft;
		static const Vector2f AnchorTopLeft;
		static const Vector2f AnchorBottomRight;
		static const Vector2f AnchorTopRight;

		/// Presets for blending factors
		enum class BlendingPreset
		{
			DISABLED, ///< uses `GL_ONE` and `GL_ZERO`
			ALPHA, ///< uses `GL_SRC_ALPHA` and `GL_ONE_MINUS_SRC_ALPHA`
			PREMULTIPLIED_ALPHA, ///< uses `GL_ONE` and `GL_ONE_MINUS_SRC_ALPHA`
			ADDITIVE, ///< uses `GL_SRC_ALPHA` and `GL_ONE`
			MULTIPLY ///< uses `GL_DST_COLOR` and `GL_ZERO`
		};

		/// OpenGL blending factors
		enum class BlendingFactor
		{
			ZERO,
			ONE,
			SRC_COLOR,
			ONE_MINUS_SRC_COLOR,
			DST_COLOR,
			ONE_MINUS_DST_COLOR,
			SRC_ALPHA,
			ONE_MINUS_SRC_ALPHA,
			DST_ALPHA,
			ONE_MINUS_DST_ALPHA,
			CONSTANT_COLOR,
			ONE_MINUS_CONSTANT_COLOR,
			CONSTANT_ALPHA,
			ONE_MINUS_CONSTANT_ALPHA,
			SRC_ALPHA_SATURATE,
		};

		/// Constructor for a drawable node with a parent and a specified relative position
		DrawableNode(SceneNode* parent, float xx, float yy);
		/// Constructor for a drawable node with a parent and a specified relative position as a vector
		DrawableNode(SceneNode* parent, const Vector2f& position);
		/// Constructor for a drawable node with a parent and positioned in the relative origin
		explicit DrawableNode(SceneNode* parent);
		/// Constructor for a drawable node with no parent and positioned in the origin
		DrawableNode();
		~DrawableNode() override;

		/// Default move constructor
		DrawableNode(DrawableNode&&);
		/// Default move assignment operator
		DrawableNode& operator=(DrawableNode&&);

		/// Updates the draw command and adds it to the queue
		bool OnDraw(RenderQueue& renderQueue) override;

		/// Returns the width of the node area
		inline virtual float width() const {
			return width_ * scaleFactor_.X;
		}
		/// Returns the height of the node area
		inline virtual float height() const {
			return height_ * scaleFactor_.Y;
		}
		/// Returns the size of the node area
		inline Vector2f size() const {
			return Vector2f(width(), height());
		}

		/// Returns the absolute width of the node area
		inline virtual float absWidth() const {
			return width_ * absScaleFactor_.X;
		}
		/// Returns the absolute height of the node area
		inline virtual float absHeight() const {
			return height_ * absScaleFactor_.Y;
		}
		/// Returns the absolute size of the node area
		inline Vector2f absSize() const {
			return Vector2f(absWidth(), absHeight());
		}

		/// Gets the transformation anchor point
		inline Vector2f anchorPoint() const {
			return (anchorPoint_ / size()) + 0.5f;
		}
		/// Sets the transformation anchor point
		void setAnchorPoint(float xx, float yy);
		/// Sets the transformation anchor point with a `Vector2f`
		inline void setAnchorPoint(const Vector2f& point) {
			setAnchorPoint(point.X, point.Y);
		}

		/// Returns true if the node renders with blending enabled
		bool isBlendingEnabled() const;
		/// Sets the blending state for node rendering
		void setBlendingEnabled(bool blendingEnabled);

		/// Returns the source blending factor
		BlendingFactor srcBlendingFactor() const;
		/// Returns the destination blending factor
		BlendingFactor destBlendingFactor() const;

		/// Sets a blending preset for source and destination blending factors
		void setBlendingPreset(BlendingPreset blendingPreset);
		/// Sets a specific source and destination blending factors
		void setBlendingFactors(BlendingFactor srcBlendingFactor, BlendingFactor destBlendingFactor);

		/// Returns the last frame in which any of the viewports have rendered this node (node was not culled)
		inline unsigned long int lastFrameRendered() const {
			return lastFrameRendered_;
		}
		/// Returns the axis-aligned bounding box of the node area in the last frame
		inline Rectf aabb() const {
			return aabb_;
		}

	protected:
		/// Node width in pixel
		float width_;
		/// Node height in pixel
		float height_;

		/// The render command class associated with this node
		RenderCommand renderCommand_;

		/// The last frame any viewports rendered this node
		unsigned long int lastFrameRendered_;
		/// Axis-aligned bounding box of the node area
		Rectf aabb_;
		/// Calculates updated values for the AABB
		virtual void updateAabb();
		/// Called by each viewport update method to update a node culling state
		void updateCulling();

		/// Protected copy constructor used to clone objects
		DrawableNode(const DrawableNode& other);

		/// Performs the required tasks upon a change to the shader
		virtual void shaderHasChanged() = 0;

		/// Updates the render command
		virtual void updateRenderCommand() = 0;

	private:
		/// Deleted assignment operator
		DrawableNode& operator=(const DrawableNode&) = delete;

		friend class ShaderState;
		friend class Viewport;
	};

}
