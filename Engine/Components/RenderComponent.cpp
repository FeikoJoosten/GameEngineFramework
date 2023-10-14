#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"

CEREAL_REGISTER_TYPE(Engine::RenderComponent)

namespace Engine {
	void RenderComponent::SetMaterial(const std::weak_ptr<Material> newMaterial) {
		material = newMaterial;
	}

	std::shared_ptr<Material> RenderComponent::GetMaterial() const {
		return material.lock();
	}

	void RenderComponent::OnComponentAdded(const std::shared_ptr<Component> addedComponent) {
		Component::OnComponentAdded(addedComponent);

		if (!transformComponent.expired()) return;
		transformComponent = std::dynamic_pointer_cast<TransformComponent>(addedComponent);
	}

	void RenderComponent::OnComponentRemoved(const std::shared_ptr<Component> removedComponent) {
		Component::OnComponentRemoved(removedComponent);

		if (transformComponent.lock().get() == removedComponent.get())
			transformComponent.reset();
		
		transformComponent = GetOwner()->GetComponent<TransformComponent>();
	}

	void RenderComponent::Render() const {
		
	}
}
