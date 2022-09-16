#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Resources/ResourceManager.hpp"

namespace Engine {
	RenderComponent::RenderComponent(const std::shared_ptr<Model> model) noexcept {
		SetModel(model);
	}

	RenderComponent::RenderComponent(const std::string& path) noexcept {
		SetModel(path);
	}

	void RenderComponent::SetModel(const std::shared_ptr<Model> newModel) {
		model = newModel;
	}

	void RenderComponent::SetModel(const std::string& path) {
		const std::weak_ptr<Model> newModel = ResourceManager::Get()->CreateModel(path, path);

		if (!newModel.expired()) {
			this->model = newModel.lock();
		}
	}

	std::shared_ptr<Model> RenderComponent::GetModel() const {
		return model;
	}

	void RenderComponent::SetMaterial(const std::weak_ptr<Material> newMaterial) {
		material = newMaterial;
	}

	std::weak_ptr<Material> RenderComponent::GetMaterial() const {
		return material;
	}

	template <typename Archive>
	void RenderComponent::Serialize(Archive& archive) {
		archive(
			cereal::virtual_base_class<Component>(this),
			CEREAL_NVP(model), 
			CEREAL_NVP(transformComponent));
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