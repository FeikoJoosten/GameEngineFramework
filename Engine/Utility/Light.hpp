#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/access.hpp>
#include <glm/glm.hpp>

/// <summary>
/// Enum containing the different types of lights that can be created.
/// </summary>
enum ENGINE_API LightType {
	LightAmbientLight = 0,
	LightDirectionalLight,
	LightPointLight,
	LightSpotLight,
	LightNonexistent = -1
};

class ENGINE_API Light {
	friend cereal::access;

public:
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 color;
	float radius;
	float attunuation;
	float coneInnerAngle;
	float coneOuterAngle;
	int padding[48];

private:

	template <class Archive>
	void Serialize(Archive& archive);
};

template <class Archive>
void Light::Serialize(Archive& archive) {
	archive(
		CEREAL_NVP(position),
		CEREAL_NVP(direction),
		CEREAL_NVP(color),
		CEREAL_NVP(radius),
		CEREAL_NVP(attunuation),
		CEREAL_NVP(coneInnerAngle),
		CEREAL_NVP(coneOuterAngle),
		CEREAL_NVP(padding)
	);
}
