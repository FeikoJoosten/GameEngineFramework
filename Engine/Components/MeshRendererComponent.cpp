#include "Engine/Components/MeshRendererComponent.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/MeshAssetImporter.hpp"

CEREAL_REGISTER_TYPE(Engine::MeshRendererComponent)

namespace Engine
{
	void MeshRendererComponent::SetMesh(const std::shared_ptr<Mesh>& newMesh) {
		mesh = newMesh;
	}

	std::shared_ptr<Mesh> MeshRendererComponent::GetMesh() const {
		return std::shared_ptr<Mesh>(mesh);
	}

	void MeshRendererComponent::Render() const {
		RenderComponent::Render();

		// TODO: Move render logic from (OpenGL)Renderer to here
	}
} //namespace Engine