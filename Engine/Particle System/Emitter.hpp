#pragma once

#include "Engine/Particle System/Particle.hpp"

namespace Engine {

	class Emitter
	{
	public:
		Emitter();
		~Emitter();

		enum class MaxParticleReachedAction {
			DELETE_OLDEST = 0,
			DELETE_NEWEST,
			DO_NOT_SPAWN
		};
/*
		enum class RandomizationType {
			LINEAR = 0,
			GAUSSIAN,
			INVERSE_GAUSSIAN
		};*/

		enum class EmitterType {
			CONTINUOUS = 0,
			BURST
		};

		enum class EmitterShape {
			SPHERE = 0,
			SPHERE_OUTLINE,
			BOX,
			BOX_OUTLINE,
			LINE,
			POINT
		};

		enum class ParticleGenerationStyle {
			CONSTANT = 0,
			RANDOM
		};

		/// <summary>
		/// Compiles the emitter and associated particle. After this, the particle can't be changed. 
		/// Any particle enum values are also no longer changeable. 
		/// NOTE: This needs to be called before the emitter can be used.
		/// </summary>
		virtual void Compile();

		/// <summary>
		/// Update the emitter.
		/// </summary>
		/// <param name="deltaTime">The time since the last call in seconds.</param>
		virtual void Update(float deltaTime);

		/// <summary>
		/// Sets the particle of the emitter.
		/// </summary>
		/// <param name="particle">The particle to set the emitter to.</param>
		void SetParticle(eastl::shared_ptr<Particle> particle);

		/// <summary>
		/// Returns a copy of the particle of the emitter.
		/// </summary>
		/// <returns>A copy of the emitter's particle.</returns>
		eastl::weak_ptr<Particle> GetParticle() const;

		/// <summary>
		/// Sets the maximum number of particles this emitter can have alive at the same time.
		/// </summary>
		/// <param name="count">The maximum number of particles.</param>
		void SetMaxParticleCount(uint32_t count);

		/// <summary>
		/// Returns the maximum number of particles this emitter can have alive at the same time.
		/// </summary>
		/// <returns>The maximum number of particles.</returns>
		uint32_t GetMaxParticleCount() const;

		/// <summary>
		/// Sets the action that should be used when trying to spawn a new particle while already having the maximum number of particles alive.
		/// </summary>
		/// <param name="action">The action to perform.</param>
		void SetMaxParticleCountReachedAction(MaxParticleReachedAction action);

		/// <summary>
		/// Returns the action that should be used when trying to spawn a new particle while already having the maximum number of particles alive.
		/// </summary>
		/// <returns>The action to perform.</returns>
		MaxParticleReachedAction GetMaxParticleCountReachedAction() const;

		/// <summary>
		/// Sets the type of the emitter.
		/// </summary>
		/// <param name="type">The new type.</param>
		void SetEmitterType(EmitterType type);

		/// <summary>
		/// Returns the type of the emitter.
		/// </summary>
		/// <returns>The emitter type.</returns>
		EmitterType GetEmitterType() const;

		/// <summary>
		/// Sets the shape of the area the emitter will emit particles in 
		/// (outline means particles will only be generated along the edges of the shape).
		/// </summary>
		/// <param name="shape">The new shape.</param>
		void SetEmitterShape(EmitterShape shape);

		/// <summary>
		/// Returns the shape of emitter.
		/// </summary>
		/// <returns>The emitter shape.</returns>
		EmitterShape GetEmitterShape() const;

		/// <summary>
		/// Sets how the particles are generated. Constant indicates that a new particle is generated every x amount of time,
		/// while random indicates that an average of x particles is generated every second.
		/// </summary>
		/// <param name="style">The style of how the particles are generated.</param>
		void SetParticleGenerationStyle(ParticleGenerationStyle style);

		/// <summary>
		/// Returns the style in which particles are generated.
		/// </summary>
		/// <returns>The particle generation style.</returns>
		ParticleGenerationStyle GetParticleGenerationStyle() const;

		/// <summary>
		/// Sets the minimum and maximum duration a burst can have. 
		/// When activated, a random duration will be picked somewhere in between these values.
		/// </summary>
		/// <param name="durationMin">The minimum duration in seconds.</param>
		/// <param name="durationMax">The minimum duration in seconds.</param>
		void SetBurstDuration(float durationMin, float durationMax);

		/// <summary>
		/// Returns the minimum and maximum duration of a burst.
		/// </summary>
		/// <returns>A pair of floats containing the durations.</returns>
		eastl::pair<float, float> GetBurstDuration();

		/// <summary>
		/// sets the minimum and maximum amount of particles spawned when the burst is triggered.
		/// When activated, a random number will be picked somewhere in between these values.
		/// </summary>
		/// <param name="particlesMin"></param>
		/// <param name="particlesMax"></param>
		void SetBurstParticleCount(float particlesMin, float particlesMax);

		/// <summary>
		/// Returns the minimum and maximum amount of particles spawned when the burst is triggered.
		/// </summary>
		/// <returns></returns>
		eastl::pair<float, float> GetBurstParticleCount();

		/// <summary>
		/// Sets the amount of particles generated by a continuous emitter. 
		/// </summary>
		/// <param name="particles">The number of particles per second.</param>
		void SetParticlesPerSecond(float particles);

		/// <summary>
		/// Returns the amount of particles generated by a continuous emitter.
		/// </summary>
		/// <returns>The number of particles per second.</returns>
		float GetParticlesPerSecond() const;

		/// <summary>
		/// Sets the radius of the emitter sphere.
		/// </summary>
		/// <param name="radius">The radius.</param>
		void SetEmitterSphereRadius(float radius);

		/// <summary>
		/// Returns the radius of the emitter sphere.
		/// </summary>
		/// <returns>The radius.</returns>
		float GetEmitterSphereRadius() const;

		/// <summary>
		/// Sets the center of the emitter sphere.
		/// </summary>
		/// <param name="center">A vec3 containing the center.</param>
		void SetEmitterSphereCenter(glm::vec3 center);

		/// <summary>
		/// Returns the center of the emitter sphere.
		/// </summary>
		/// <returns>A vec3 containing the center.</returns>
		glm::vec3 GetEmitterSphereCenter() const;

		/// <summary>
		/// Sets the corners of the box emitter.
		/// </summary>
		/// <param name="corner1">A vec3 containing the first corner.</param>
		/// <param name="corner2">A vec3 containing the second corner.</param>
		void SetEmitterBox(glm::vec3 corner1, glm::vec3 corner2);

		/// <summary>
		/// Returns the corners of the emitter box.
		/// </summary>
		/// <returns>A pair of vec3s containing the corners.</returns>
		eastl::pair<glm::vec3, glm::vec3> GetEmitterBox();

		/// <summary>
		/// Sets the start and end of the emitter line.
		/// </summary>
		/// <param name="start">The start position.</param>
		/// <param name="end">The end position.</param>
		void SetEmitterLine(glm::vec3 start, glm::vec3 end);

		/// <summary>
		/// Returns the start and end of the emitter line.
		/// </summary>
		/// <returns>A pair of vec3s containing the start and end.</returns>
		eastl::pair<glm::vec3, glm::vec3> GetEmitterLine();

		/// <summary>
		/// Sets the location of a point emitter.
		/// </summary>
		/// <param name="point">The postion of the emitter.</param>
		void SetEmitterPointLocation(glm::vec3 point);

		/// <summary>
		/// Returns the location of a point emitter.
		/// </summary>
		/// <returns>A vec3 containing the position.</returns>
		glm::vec3 GetEmitterPointLocation() const;


	protected:
		eastl::shared_ptr<Particle> particle;

		uint32_t maxParticleCount;

		MaxParticleReachedAction maxParticleReachedAction;

		EmitterType emitterType;

		EmitterShape emitterShape;

		ParticleGenerationStyle particleGenerationStyle;

		bool active;

		bool compiled;

		float particlesPerSecond;

		float burstParticleCountMin, burstParticleCountMax;

		float burstParticles;

		float burstParticlesPerSecond;

		float burstDurationMin, burstDurationMax;

		float burstDuration;

		float burstActiveTime;

		float sphereRadius;

		glm::vec3 sphereCenter;

		glm::vec3 boxCorner1, boxCorner2;

		glm::vec3 lineStart, lineEnd;

		glm::vec3 pointLocation;
		
	};

}