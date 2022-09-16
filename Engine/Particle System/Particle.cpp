#define GLM_FORCE_SWIZZLE

#include "Engine/Particle System/Particle.hpp"

#include "glm/gtc/random.hpp"

namespace Engine {

	Particle::Particle(): particleType(), particleBlendMode(), colorInterpolateType(), lifetimeMin(0), lifetimeMax(0),
	                      speedMin(0), speedMax(0), massMin(0), massMax(0), maxSegments(0), newSegmentsPerFrame(0),
	                      lineEndAction(), startLineThicknessMin(0), startLineThicknessMax(0), endLineThicknessMin(0),
	                      endLineThicknessMax(0), lineThicknessInterpolateType(), sizeInterpolateType(),
	                      startRollSpeedMin(0), startRollSpeedMax(0), endRollSpeedMin(0), endRollSpeedMax(0),
	                      rollSpeedInterpolateType(), rollMin(0), rollMax(0), scaleInterpolateType(),
	                      rotationSpeedInterpolateType(), meshLit(false), castShadows(false)
	{
	}


	Particle::~Particle()
	{
	}

	void Particle::SetParticleType(ParticleType type)
	{
		particleType = type;
	}

	Particle::ParticleType Particle::GetParticleType() const
	{
		return particleType;
	}

	void Particle::SetBlendMode(BlendMode mode)
	{
		particleBlendMode = mode;
	}

	Particle::BlendMode Particle::GetParticleBlendMode() const
	{
		return particleBlendMode;
	}

	void Particle::SetStartColor(glm::vec4 colorMin, glm::vec4 colorMax)
	{
		startColorMin = colorMin;
		startColorMax = colorMax;
	}

	std::pair<glm::vec4, glm::vec4> Particle::GetStartColor()
	{
		return std::pair<glm::vec4, glm::vec4>(startColorMin, startColorMax);
	}

	void Particle::SetEndColor(glm::vec4 colorMin, glm::vec4 colorMax)
	{
		endColorMin = colorMin;
		endColorMax = colorMax;
	}

	std::pair<glm::vec4, glm::vec4> Particle::GetEndColor()
	{
		return std::pair<glm::vec4, glm::vec4>(endColorMin, endColorMax);
	}

	void Particle::SetColorInterpolateType(InterpolateType type)
	{
		colorInterpolateType = type;
	}

	Particle::InterpolateType Particle::GetColorInterpolateType() const
	{
		return colorInterpolateType;
	}

	void Particle::SetForce(glm::vec3 forceMin, glm::vec3 forceMax)
	{
		this->forceMin = glm::vec4(forceMin, 0.f);
		this->forceMax = glm::vec4(forceMax, 0.f);
	}

	std::pair<glm::vec3, glm::vec3> Particle::GetForce()
	{
		return std::pair<glm::vec3, glm::vec3>(forceMin.xyz, forceMax.xyz);
	}

	void Particle::SetLifetime(float lifetimeMin, float lifetimeMax)
	{
		this->lifetimeMin = lifetimeMin;
		this->lifetimeMax = lifetimeMax;
	}

	std::pair<float, float> Particle::GetLifetime()
	{
		return std::pair<float, float>(lifetimeMin, lifetimeMax);
	}

	void Particle::SetSpeed(float speedMin, float speedMax)
	{
		this->speedMin = speedMin;
		this->speedMax = speedMax;
	}

	std::pair<float, float> Particle::GetSpeed()
	{
		return std::pair<float, float>(speedMin, speedMax);
	}

	void Particle::SetMass(float massMin, float massMax)
	{
		this->massMin = massMin;
		this->massMax = massMax;
	}

	std::pair<float, float> Particle::GetMass()
	{
		return std::pair<float, float>(massMin, massMax);
	}

	void Particle::SetGravityEnabled(bool gravity)
	{
		this->gravity = gravity;
	}

	bool Particle::IsGravityEnabled() const
	{
		return gravity;
	}

	void Particle::SetTexture(std::weak_ptr<Texture> texture)
	{
		texture = texture;
		if (texture.lock() != std::shared_ptr<Texture>(nullptr))
			textured = true;
		else
			textured = false;
	}

	std::weak_ptr<Texture> Particle::GetTexture() const
	{
		return texture;
	}

	bool Particle::HasTexture() const
	{
		return textured;
	}

	void Particle::SetLineMaxSegments(int segments)
	{
		maxSegments = segments;
	}

	int Particle::GetLineMaxSegments() const
	{
		return maxSegments;
	}

	void Particle::SetNewSegmentsPerFrame(int segments)
	{
		newSegmentsPerFrame = segments;
	}

	int Particle::GetNewSegmentsPerFrame() const
	{
		return newSegmentsPerFrame;
	}

	void Particle::SetLineEndAction(LineEndAction action)
	{
		lineEndAction = action;
	}

	Particle::LineEndAction Particle::GetLineEndAction() const
	{
		return lineEndAction;
	}

	void Particle::SetLineStartThickness(float thicknessMin, float thicknessMax)
	{
		startLineThicknessMin = thicknessMin;
		startLineThicknessMax = thicknessMax;
	}

	std::pair<float, float> Particle::GetLineStartThickness()
	{
		return std::pair<float, float>(startLineThicknessMin, startLineThicknessMax);
	}

	void Particle::SetLineEndthickness(float thicknessMin, float thicknessMax)
	{
		endLineThicknessMin = thicknessMin;
		endLineThicknessMax = thicknessMax;
	}

	std::pair<float, float> Particle::GetLineEndThickness()
	{
		return std::pair<float, float>(endLineThicknessMin, endLineThicknessMax);
	}

	void Particle::SetLineThicknessInterpolateType(InterpolateType type)
	{
		lineThicknessInterpolateType = type;
	}

	Particle::InterpolateType Particle::GetLineThicknessInterpolateType() const
	{
		return lineThicknessInterpolateType;
	}

	void Particle::SetBillboardSizeStart(glm::vec2 sizeMin, glm::vec2 sizeMax)
	{
		startSizeMin = sizeMin;
		startSizeMax = sizeMax;
	}

	std::pair<glm::vec2, glm::vec2> Particle::GetBillboardSizeStart()
	{
		return std::pair<glm::vec2, glm::vec2>(startSizeMin, startSizeMax);
	}

	void Particle::SetBillboardSizeEnd(glm::vec2 sizeMin, glm::vec2 sizeMax)
	{
		endSizeMin = sizeMin;
		endSizeMax = sizeMax;
	}

	std::pair<glm::vec2, glm::vec2> Particle::GetBillboardSizeEnd()
	{
		return std::pair<glm::vec2, glm::vec2>(endSizeMin, endSizeMax);
	}

	void Particle::SetBillboardSizeInterpolateType(InterpolateType type)
	{
		sizeInterpolateType = type;
	}

	Particle::InterpolateType Particle::GetBillboardSizeInterpolateType() const
	{
		return sizeInterpolateType;
	}

	void Particle::SetBillboardRollSpeedStart(float speedMin, float speedMax)
	{
		startRollSpeedMin = speedMin;
		startRollSpeedMax = speedMax;
	}

	std::pair<float, float> Particle::GetBillboardRollSpeedStart()
	{
		return std::pair<float, float>(startRollSpeedMin, startRollSpeedMax);
	}

	void Particle::SetBillboardRollSpeedEnd(float speedMin, float speedMax)
	{
		endRollSpeedMin = speedMin;
		endRollSpeedMax = speedMax;
	}

	std::pair<float, float> Particle::GetBillboardRollSpeedEnd()
	{
		return std::pair<float, float>(endRollSpeedMin, endRollSpeedMax);
	}

	void Particle::SetBillboardRoll(float rollMin, float rollMax)
	{
		this->rollMin = rollMin;
		this->rollMax = rollMax;
	}

	std::pair<float, float> Particle::GetBillboardRoll()
	{
		return std::pair<float, float>(rollMin, rollMax);
	}

	void Particle::SetBillboardRollSpeedInterpolateType(InterpolateType type)
	{
		rollSpeedInterpolateType = type;
	}

	Particle::InterpolateType Particle::GetBillboardRollSpeedInterpolateType() const
	{
		return rollSpeedInterpolateType;
	}

	void Particle::SetMesh(std::weak_ptr<Mesh> mesh)
	{
		this->mesh = mesh;
	}

	std::weak_ptr<Mesh> Particle::GetMesh() const
	{
		return mesh;
	}

	void Particle::SetMaterial(std::weak_ptr<Material> material)
	{
		this->material = material;
	}

	std::weak_ptr<Material> Particle::GetMaterial() const
	{
		return material;
	}

	void Particle::SetMeshScaleStart(glm::vec3 scaleMin, glm::vec3 scaleMax)
	{
		startScaleMin = glm::vec4(scaleMin, 0.f);
		startScaleMax = glm::vec4(scaleMax, 0.f);
	}

	std::pair<glm::vec3, glm::vec3> Particle::GetMeshScaleStart()
	{
		return std::pair<glm::vec3, glm::vec3>(startScaleMin.xyz, startScaleMax.xyz);
	}

	void Particle::SetMeshScaleEnd(glm::vec3 scaleMin, glm::vec3 scaleMax)
	{
		endScaleMin = glm::vec4(scaleMin, 0.f);
		endScaleMax = glm::vec4(scaleMax, 0.f);
	}

	std::pair<glm::vec3, glm::vec3> Particle::GetMeshScaleEnd()
	{
		return std::pair<glm::vec3, glm::vec3>(endScaleMin.xyz, endScaleMax.xyz);
	}

	void Particle::SetMeshScaleInterpolateType(InterpolateType type)
	{
		scaleInterpolateType = type;
	}

	Particle::InterpolateType Particle::GetMeshScaleInterpolateType() const
	{
		return scaleInterpolateType;
	}

	void Particle::SetMeshRotationSpeedStart(glm::vec3 speedMin, glm::vec3 speedMax)
	{
		startRotationSpeedMin = glm::vec4(speedMin, 0.f);
		startRotationSpeedMax = glm::vec4(speedMax, 0.f);
	}

	std::pair<glm::vec3, glm::vec3> Particle::GetMeshRotationSpeedStart()
	{
		return std::pair<glm::vec3, glm::vec3>(startRotationSpeedMin.xyz, startRotationSpeedMax.xyz);
	}

	void Particle::SetMeshRotationSpeedEnd(glm::vec3 speedMin, glm::vec3 speedMax)
	{
		endRotationSpeedMin = glm::vec4(speedMin, 0.f);
		endRotationSpeedMax = glm::vec4(speedMax, 0.f);
	}

	std::pair<glm::vec3, glm::vec3> Particle::GetMeshRotationSpeedEnd()
	{
		return std::pair<glm::vec3, glm::vec3>(endRotationSpeedMin.xyz, endRotationSpeedMax.xyz);
	}

	void Particle::SetMeshRotation(glm::vec3 rotationMin, glm::vec3 rotationMax)
	{
		this->rotationMin = glm::vec4(rotationMin, 0.f);
		this->rotationMax = glm::vec4(rotationMax, 0.f);
	}

	std::pair<glm::vec3, glm::vec3> Particle::GetMeshRotation()
	{
		return std::pair<glm::vec3, glm::vec3>(rotationMin.xyz, rotationMax.xyz);
	}

	void Particle::SetMeshRotationSpeedInterpolateType(InterpolateType type)
	{
		rotationSpeedInterpolateType = type;
	}

	Particle::InterpolateType Particle::GetMeshRotationSpeedInterpolateType() const
	{
		return rotationSpeedInterpolateType;
	}

	void Particle::SetMeshLit(bool lit)
	{
		meshLit = lit;
	}

	bool Particle::IsMeshLit() const
	{
		return meshLit;
	}

	void Particle::SetCastsShadows(bool castShadows)
	{
		this->castShadows = castShadows;
	}

	bool Particle::CanCastShadows() const
	{
		return castShadows;
	}

	Particle::ParticleData Particle::CreateParticle(glm::vec4 position, glm::vec4 direction) const
	{
		ParticleData ret = {};
		ret.startColor = glm::linearRand(startColorMin, startColorMax);
		ret.endColor = glm::linearRand(endColorMin, endColorMax);
		if (colorInterpolateType == InterpolateType::END)
			ret.currentColor = ret.endColor;
		else
			ret.currentColor = ret.startColor;
		ret.position = position;
		ret.direction = direction;
		ret.force = glm::linearRand(forceMin, forceMax);
		ret.lifetime = RandomFloatLinear(lifetimeMin, lifetimeMax);
		ret.currentLifetime = 0.f;
		ret.speed = RandomFloatLinear(speedMin, speedMax);
		ret.mass = RandomFloatLinear(speedMin, speedMax);

		return ret;
	}

	Particle::LineData Particle::CreateParticleLineData() const
	{
		LineData ret;
		ret.segmentCount = 0;
		ret.startThickness = RandomFloatLinear(startLineThicknessMin, startLineThicknessMax);
		ret.endThickness = RandomFloatLinear(endLineThicknessMin, endLineThicknessMax);
		if (lineThicknessInterpolateType == InterpolateType::END)
			ret.currentThickness = ret.endThickness;
		else
			ret.currentThickness = ret.startThickness;
		return ret;
	}

	Particle::BillboardData Particle::CreateParticleBillboardData() const
	{
		BillboardData ret = {};
		ret.startSize = glm::linearRand(startSizeMin, startSizeMax);
		ret.endSize = glm::linearRand(endSizeMin, endSizeMax);
		if (sizeInterpolateType == InterpolateType::END)
			ret.currentSize = ret.endSize;
		else
			ret.currentSize = ret.startSize;
		ret.startRotationSpeed = RandomFloatLinear(startRollSpeedMin, startRollSpeedMax);
		ret.endRotationSpeed = RandomFloatLinear(endRollSpeedMin, endRollSpeedMax);
		if (rollSpeedInterpolateType == InterpolateType::END)
			ret.currentRotationSpeed = ret.endRotationSpeed;
		else
			ret.currentRotationSpeed = ret.startRotationSpeed;
		ret.currentRotation = RandomFloatLinear(rollMin, rollMax);

		return ret;
	}

	Particle::MeshData Particle::CreateParticleMeshData() const
	{
		MeshData ret = {};
		ret.startScale = glm::linearRand(startScaleMin, startScaleMax);
		ret.endScale = glm::linearRand(endScaleMin, endScaleMax);
		if (scaleInterpolateType == InterpolateType::END)
			ret.currentScale = ret.endScale;
		else
			ret.currentScale = ret.startScale;
		ret.startRotationSpeed = glm::linearRand(startRotationSpeedMin, startRotationSpeedMax);
		ret.endRotationSpeed = glm::linearRand(endRotationSpeedMin, endRotationSpeedMax);
		if (rotationSpeedInterpolateType == InterpolateType::END)
			ret.currentRotationSpeed = ret.endRotationSpeed;
		else
			ret.currentRotationSpeed = ret.startRotationSpeed;
		ret.currentRotation = glm::linearRand(rotationMin, rotationMax);

		return ret;
	}

	float Particle::RandomFloatLinear(float min, float max) const
	{
		float random = (static_cast<float>(rand())) / static_cast<float>(RAND_MAX);
		float diff = max - min;
		float r = random * diff;
		return min + r;
	}

}