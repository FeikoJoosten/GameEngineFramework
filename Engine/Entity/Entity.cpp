#include "Engine/Entity/Entity.hpp"
#include "Engine/Components/Component.hpp"

namespace Engine {
	Entity::~Entity() {
		OnEntityDestroyedEvent(std::static_pointer_cast<Entity>(shared_from_this()));
	}

	std::vector<std::shared_ptr<Component>> Entity::GetAllComponents() const {
		return components;
	}

	void Entity::Update() const {
		if (isActive == false)
			return;

		for (const std::shared_ptr<Component>& component : components) {
			if (!component->GetIsEnabled())
				continue;

			component->Update();
		}
	}

	bool Entity::GetIsActive() const {
		return isActive;
	}

	void Entity::SetIsActive(const bool newIsActive) {
		if (newIsActive == isActive) return;

		isActive = newIsActive;
		OnActiveStateChangedEvent(std::static_pointer_cast<Entity>(shared_from_this()), isActive);
	}

	void Entity::OnEntityAddedToEntitySystem() {
		const std::shared_ptr<Entity> pointerReference = std::static_pointer_cast<Entity>(shared_from_this());
		for(const std::shared_ptr<Component>& component : components) {
			component->SetOwner(std::static_pointer_cast<Entity>(shared_from_this()));
			OnComponentAddedEvent(pointerReference, component);
		}
	}

	void Entity::InitializeComponent(const std::shared_ptr<Component>& componentToInitialize) {
		componentToInitialize->SetOwner(std::static_pointer_cast<Entity>(shared_from_this()));
		OnComponentAdded(componentToInitialize);

		for (size_t j = 0, size = components.size() - 1; j < size; ++j)
			componentToInitialize->OnComponentAdded(components[j]);
	}

	void Entity::OnComponentAdded(const std::shared_ptr<Component>& addedComponent) const {
		// No need to inform the last added component, that has just been added.
		for (size_t i = 0, size = components.size() - 1; i < size; ++i)
			components[i]->OnComponentAdded(addedComponent);
	}

	void Entity::OnComponentRemoved(const std::shared_ptr<Component>& removedComponent) const {
		for (const std::shared_ptr<Component>& component : components)
			component->OnComponentRemoved(removedComponent);
	}
} // namespace Engine
