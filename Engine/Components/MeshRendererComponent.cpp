#include "Engine/Components/MeshRendererComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"

CEREAL_REGISTER_TYPE(Engine::MeshRendererComponent)

namespace Engine
{
	void MeshRendererComponent::SetMesh(const std::shared_ptr<Mesh>& newMesh) {
		if (mesh == newMesh) return;

		const std::shared_ptr<Mesh> oldMesh = mesh;
		mesh = newMesh;

		OnMeshChangedEvent(std::static_pointer_cast<MeshRendererComponent>(shared_from_this()), oldMesh, mesh);
	}

	std::shared_ptr<Mesh> MeshRendererComponent::GetMesh() const {
		return std::shared_ptr<Mesh>(mesh);
	}

	void MeshRendererComponent::Render(const std::shared_ptr<CameraComponent>& camera) const {
		if (!mesh) return;

		const std::shared_ptr<Material>& material = GetMaterial();
		if (!material) return;

		const std::shared_ptr<TransformComponent>& transform = GetComponent<TransformComponent>(); // TODO: Cache this
		if (!transform) return;

		Renderer::RenderMesh(mesh, camera, material, transform->GetMatrix());

	}
}