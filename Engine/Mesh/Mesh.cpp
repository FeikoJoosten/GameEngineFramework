#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Engine/Engine.hpp"

CEREAL_REGISTER_TYPE(Engine::Mesh);

namespace Engine
{
	const std::vector<glm::vec3>& Mesh::GetVertices() const {
		return vertices;
	}

	const std::vector<glm::vec3>& Mesh::GetNormals() const {
		return normals;
	}

	const std::vector<unsigned>& Mesh::GetIndices() const {
		return indices;
	}

	const std::vector<glm::vec2>& Mesh::GetUv() const {
		return uvs;
	}

	const std::vector<glm::vec2>& Mesh::GetUv2() const {
		return uv2;
	}

	const std::vector<glm::vec2>& Mesh::GetUv3() const {
		return uv3;
	}

	const std::vector<glm::vec2>& Mesh::GetUv4() const {
		return uv4;
	}

	const std::vector<glm::vec2>& Mesh::GetUv5() const {
		return uv5;
	}

	const std::vector<glm::vec2>& Mesh::GetUv6() const {
		return uv6;
	}

	const std::vector<glm::vec2>& Mesh::GetUv7() const {
		return uv7;
	}

	const std::vector<glm::vec2>& Mesh::GetUv8() const {
		return uv8;
	}
}
