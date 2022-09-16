#pragma once

#include "glm/glm.hpp"
#include "Engine/Api.hpp"

/// <summary>
/// Enum containing the different types of lights that can be created.
/// </summary>
enum ENGINE_API LightType {
	LIGHT_AMBIENT_LIGHT = 0,
	LIGHT_DIRECTIONAL_LIGHT,
	LIGHT_POINT_LIGHT,
	LIGHT_SPOT_LIGHT,
	LIGHT_NONEXISTENT = -1
};

class ENGINE_API Light {
public:
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 color;
	float radius;
	float attunuation;
	float coneInnerAngle;
	float coneOuterAngle;
	int padding[48];
};
