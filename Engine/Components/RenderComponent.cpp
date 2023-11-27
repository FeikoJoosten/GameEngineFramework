#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"

CEREAL_REGISTER_TYPE(Engine::RenderComponent)

namespace Engine {
	void RenderComponent::SetMaterial(const xg::Guid& newMaterialGuid) {
		if (material == newMaterialGuid) return;

		const std::shared_ptr<Material> oldMaterial = material;
		material = newMaterialGuid;

		OnMaterialChangedEvent(std::static_pointer_cast<RenderComponent>(shared_from_this()), oldMaterial, material);
	}

	void RenderComponent::SetMaterial(const std::shared_ptr<Material>& newMaterial) {
		if (material == newMaterial) return;

		const std::shared_ptr<Material> oldMaterial = material;
		material = newMaterial;
		
		OnMaterialChangedEvent(std::static_pointer_cast<RenderComponent>(shared_from_this()), oldMaterial, material);
	}

	std::shared_ptr<Material> RenderComponent::GetMaterial() const {
		return material;
	}
}
