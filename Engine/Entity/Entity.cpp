#include "Engine/Entity/Entity.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Entity/EntitySystem.hpp"

namespace Engine {
	Entity::Entity(std::string name) : id(-1), name(std::move(name)), isActive(true) {}

	std::vector<std::shared_ptr<Component>> Entity::GetAllComponents() const {
		return components;
	}

	std::weak_ptr<Entity> Entity::GetPointer() const {
		return EntitySystem::Get()->GetEntity(id).lock();
	}

	void Entity::Update() {
		if (isActive == false)
			return;

		for (size_t i = 0, size = components.size(); i < size; ++i) {
			if (components[i]->isEnabled == false)
				continue;

			components[i]->Update();
		}
	}

	size_t Entity::ComponentCount() const {
		return components.size();
	}

	int Entity::GetId() const {
		return id;
	}

	bool Entity::GetIsActive() const {
		return isActive;
	}

	void Entity::SetIsActive(const bool newIsActive) {
		if (newIsActive == isActive) return;

		isActive = newIsActive;
		OnActiveStateChangedEvent(GetPointer().lock(), isActive);
	}

	void Entity::SetId(const int newId) {
		id = newId;
	}

	void Entity::OnComponentAdded(const std::shared_ptr<Component> addedComponent) const {
		// No need to inform the last added component, that has just been added.
		for (size_t i = 0, size = components.size() - 1; i < size; ++i)
			components[i]->OnComponentAdded(addedComponent);
	}

	void Entity::OnComponentRemoved(const std::shared_ptr<Component> removedComponent) const {
		for (size_t i = 0, size = components.size(); i < size; ++i)
			components[i]->OnComponentRemoved(removedComponent);
	}
} // namespace Engine