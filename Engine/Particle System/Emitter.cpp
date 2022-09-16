#include "Engine/Particle System/Emitter.hpp"

namespace Engine {

	Emitter::Emitter()
	{
		compiled = false;
		active = false;
	}


	Emitter::~Emitter()
	{
	}

	void Emitter::Compile()
	{
		compiled = true;
	}

	void Emitter::Update(float deltaTime)
	{
		if (!compiled)
			return;
	}

	void Emitter::SetParticle(std::shared_ptr<Particle> particle)
	{
		if (!compiled)
			particle = particle;
	}

	std::weak_ptr<Particle> Emitter::GetParticle() const
	{
		return particle;
	}

	void Emitter::SetMaxParticleCount(uint32_t count)
	{
		if (!compiled)
			maxParticleCount = count;
	}

	uint32_t Emitter::GetMaxParticleCount() const
	{
		return maxParticleCount;
	}

	void Emitter::SetMaxParticleCountReachedAction(MaxParticleReachedAction action)
	{
		maxParticleReachedAction = action;
	}

	Emitter::MaxParticleReachedAction Emitter::GetMaxParticleCountReachedAction() const
	{
		return maxParticleReachedAction;
	}

	void Emitter::SetEmitterType(EmitterType type)
	{
		emitterType = type;
	}

	Emitter::EmitterType Emitter::GetEmitterType() const
	{
		return emitterType;
	}

	void Emitter::SetEmitterShape(EmitterShape shape)
	{
		emitterShape = shape;
	}

	Emitter::EmitterShape Emitter::GetEmitterShape() const
	{
		return emitterShape;
	}

	void Emitter::SetParticleGenerationStyle(ParticleGenerationStyle style)
	{
		particleGenerationStyle = style;
	}

	Emitter::ParticleGenerationStyle Emitter::GetParticleGenerationStyle() const
	{
		return particleGenerationStyle;
	}

	void Emitter::SetBurstDuration(float durationMin, float durationMax)
	{
		burstDurationMin = durationMin;
		burstDurationMax = durationMax;
	}

	std::pair<float, float> Emitter::GetBurstDuration()
	{
		return std::pair<float, float>(burstDurationMin, burstDurationMax);
	}

	void Emitter::SetBurstParticleCount(float particlesMin, float particlesMax)
	{
		burstParticleCountMin = particlesMin;
		burstParticleCountMax = particlesMax;
	}

	std::pair<float, float> Emitter::GetBurstParticleCount()
	{
		return std::pair<float, float>(burstParticleCountMin, burstParticleCountMax);
	}

	void Emitter::SetParticlesPerSecond(float particles)
	{
		particlesPerSecond = particles;
	}

	float Emitter::GetParticlesPerSecond() const
	{
		return particlesPerSecond;
	}

	void Emitter::SetEmitterSphereRadius(float radius)
	{
		sphereRadius = radius;
	}

	float Emitter::GetEmitterSphereRadius() const
	{
		return sphereRadius;
	}

	void Emitter::SetEmitterSphereCenter(glm::vec3 center)
	{
		sphereCenter = center;
	}

	glm::vec3 Emitter::GetEmitterSphereCenter() const
	{
		return sphereCenter;
	}

	void Emitter::SetEmitterBox(glm::vec3 corner1, glm::vec3 corner2)
	{
		boxCorner1.x = corner1.x <= corner2.x ? corner1.x : corner2.x;
		boxCorner2.x = corner1.x > corner2.x ? corner1.x : corner2.x;

		boxCorner1.y = corner1.y <= corner2.y ? corner1.y : corner2.y;
		boxCorner2.y = corner1.y > corner2.y ? corner1.y : corner2.y;

		boxCorner1.z = corner1.z <= corner2.z ? corner1.z : corner2.z;
		boxCorner2.z = corner1.z > corner2.z ? corner1.z : corner2.z;


	}

	std::pair<glm::vec3, glm::vec3> Emitter::GetEmitterBox()
	{
		return std::pair<glm::vec3, glm::vec3>(boxCorner1, boxCorner2);
	}

	void Emitter::SetEmitterLine(glm::vec3 start, glm::vec3 end)
	{
		lineStart = start;
		lineEnd = end;
	}

	std::pair<glm::vec3, glm::vec3> Emitter::GetEmitterLine()
	{
		return std::pair<glm::vec3, glm::vec3>(lineStart, lineEnd);
	}

	void Emitter::SetEmitterPointLocation(glm::vec3 point)
	{
		pointLocation = point;
	}

	glm::vec3 Emitter::GetEmitterPointLocation() const
	{
		return pointLocation;
	}

}