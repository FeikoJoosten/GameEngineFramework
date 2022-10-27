#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Resources/ResourceManager.hpp"

namespace Engine {
	void RenderComponent::SetModel(const std::shared_ptr<Model> newModel) {
		model = newModel;
	}

	void RenderComponent::SetModel(const std::string& path) {
		if (const std::weak_ptr<Model> newModel = ResourceManager::Get()->CreateModel(path, path); !newModel.expired()) {
			this->model = newModel.lock();
		}
	}

	std::shared_ptr<Model> RenderComponent::GetModel() const {
		return model;
	}

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
		
		transformComponent = GetComponent<TransformComponent>();
	}
}