#pragma once

#include "Engine/Api/Api.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace Engine {
	class Mesh;
	class Material;

	struct ENGINE_API RenderCommand {
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		glm::mat4x4 modelMatrix;

		RenderCommand(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4x4& modelMatrix);

		[[nodiscard]] std::shared_ptr<Mesh> GetMesh() const;

		[[nodiscard]] std::shared_ptr<Material> GetMaterial() const;

		[[nodiscard]] glm::mat4x4 GetModelMatrix() const;
	};
}

