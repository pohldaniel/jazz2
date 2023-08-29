#pragma once

#include "Sprite.h"
#include "../Primitives/Vector2.h"
#include "../Primitives/Color.h"

namespace nCine
{
	class Texture;

	/// The class for a single particle data
	class Particle : public Sprite
	{
	public:
		/// Current particle remaining life in seconds
		float life_;
		/// Initial particle remaining life
		float startingLife; // for affectors
		/// Initial particle rotation
		float startingRotation; // for affectors
		/// Current particle velocity vector
		Vector2f velocity_;
		/// A flag indicating if particle transformations are in local space or not
		bool inLocalSpace_;

		/// Constructor for a particle with a parent and texture, positioned in the relative origin
		Particle(SceneNode* parent, Texture* texture);

		/// Default move constructor
		Particle(Particle&&) = default;
		/// Default move assignment operator
		Particle& operator=(Particle&&) = default;

		/// Returns true if the particle is still alive
		inline bool isAlive() const {
			return life_ > 0.0f;
		}

	protected:
		/// Returns a copy of this object
		/*! \note This method is protected as it should only be called by a `ParticleSystem` */
		inline Particle clone() const {
			return Particle(*this);
		}

		/// Protected copy constructor used to clone objects
		Particle(const Particle& other);

	private:
		/// Initializes a particle with initial life, position, velocity and rotation
		void init(float life, Vector2f pos, Vector2f vel, float rot, bool inLocalSpace);
		/// Updates particle data after the specified amount of seconds has passed
		void OnUpdate(float timeMult) override;
		/// Custom transform method to allow independent position from parent
		void transform() override;

		/// Deleted assignment operator
		Particle& operator=(const Particle&) = delete;

		friend class ParticleSystem;
	};

}
