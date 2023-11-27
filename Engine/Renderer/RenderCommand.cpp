#include "Engine/Renderer/RenderCommand.hpp"

namespace Engine {

	RenderCommand::RenderCommand(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4x4& modelMatrix) : mesh(std::move(mesh)),	material(std::move(material)), modelMatrix(modelMatrix) {}

	std::shared_ptr<Mesh> RenderCommand::GetMesh() const {
		return mesh;
	}

	std::shared_ptr<Material> RenderCommand::GetMaterial() const {
		return material;
	}

	glm::mat4x4 RenderCommand::GetModelMatrix() const {
		return modelMatrix;
	}
}
