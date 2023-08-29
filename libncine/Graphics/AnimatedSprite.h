#pragma once

#include "Sprite.h"
#include "RectAnimation.h"

#include <Containers/SmallVector.h>

using namespace Death::Containers;

namespace nCine
{
	/// A class for an animated sprite
	class AnimatedSprite : public Sprite
	{
	public:
		/// Default constructor for an animated sprite with no parent and no texture, positioned in the origin
		AnimatedSprite();
		/// Constructor for an animated sprite with a parent and texture, positioned in the relative origin
		AnimatedSprite(SceneNode* parent, Texture* texture);
		/// Constructor for an animated sprite with a texture but no parent, positioned in the origin
		explicit AnimatedSprite(Texture* texture);
		/// Constructor for an animated sprite with a parent, a texture and a specified relative position
		AnimatedSprite(SceneNode* parent, Texture* texture, float xx, float yy);
		/// Constructor for an animated sprite with a parent, a texture and a specified relative position as a vector
		AnimatedSprite(SceneNode* parent, Texture* texture, const Vector2f& position);
		/// Constructor for an animated sprite with a texture and a specified position but no parent
		AnimatedSprite(Texture* texture, float xx, float yy);
		/// Constructor for an animated sprite with a texture and a specified position as a vector but no parent
		AnimatedSprite(Texture* texture, const Vector2f& position);

		/// Default move constructor
		AnimatedSprite(AnimatedSprite&&) = default;
		/// Default move assignment operator
		AnimatedSprite& operator=(AnimatedSprite&&) = default;

		/// Returns a copy of this object
		inline AnimatedSprite clone() const {
			return AnimatedSprite(*this);
		}

		/// Returns true if the current animation is paused
		bool isPaused() const;
		/// Sets the pause state for the animation
		void setPaused(bool isPaused);

		void OnUpdate(float timeMult) override;

		/// Adds a new animation
		void addAnimation(const RectAnimation& anim);
		/// Adds a new animation with move semantics
		void addAnimation(RectAnimation&& anim);
		/// Deletes all animations
		void clearAnimations();

		/// Returns the number of animations
		inline unsigned int numAnimations() {
			return (unsigned int)anims_.size();
		}
		/// Returns the array of all animations
		inline SmallVectorImpl<RectAnimation>& animations() {
			return anims_;
		}
		/// Returns the constant array of all animations
		inline const SmallVectorImpl<RectAnimation>& animations() const {
			return anims_;
		}

		/// Returns the index of the current animation
		unsigned int animationIndex() const {
			return currentAnimIndex_;
		}
		/// Sets current animation index and its frame number
		void setAnimationIndex(unsigned int animIndex);

		/// Returns the current animation, if any
		RectAnimation* currentAnimation();
		/// Returns the constant version of current animation, if any
		const RectAnimation* currentAnimation() const;

		/// Returns the frame number of current animation
		unsigned int frame() const;
		/// Sets current animation to a specified frame number
		void setFrame(unsigned int frameNum);

		inline static ObjectType sType() {
			return ObjectType::AnimatedSprite;
		}

	protected:
		/// Protected copy constructor used to clone objects
		AnimatedSprite(const AnimatedSprite& other);

	private:
		SmallVector<RectAnimation, 0> anims_;
		unsigned int currentAnimIndex_;

		/// Deleted assignment operator
		AnimatedSprite& operator=(const AnimatedSprite&) = delete;
	};

}
