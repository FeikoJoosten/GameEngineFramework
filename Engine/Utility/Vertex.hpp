#pragma once
#include <glm/glm.hpp>
/// <summary>
/// This object is used to store general data for a vertex.
/// </summary>
class Vertex
{
public:
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	float boneWeights[4];
	uint32_t boneIds[4];
};