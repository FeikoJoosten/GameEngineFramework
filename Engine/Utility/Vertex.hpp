#pragma once

#include "Engine/Api/Api.hpp"

#include <glm/glm.hpp>
/// <summary>
/// This object is used to store general data for a vertex.
/// </summary>
class ENGINE_API Vertex
{
public:
	glm::vec3 vertices;
	glm::vec3 normal;
	glm::vec2 texCoords;
	float boneWeights[4];
	uint32_t boneIds[4];
};