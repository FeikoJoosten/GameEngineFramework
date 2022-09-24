#include "Engine/Entity/Entity.hpp"
#include "Engine/Components/Component.hpp"

namespace Engine {
	Entity::Entity(std::string name) : id(-1), isActive(true), name(std::move(name)) {}

	Entity::~Entity() {
		OnEntityDestroyedEvent(GetPointer());
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

	int Entity::GetId() const {
		return id;
	}

	bool Entity::GetIsActive() const {
		return isActive;
	}

	void Entity::SetIsActive(const bool newIsActive) {
		if (newIsActive == isActive) return;

		isActive = newIsActive;
		OnActiveStateChangedEvent(GetPointer(), isActive);
	}

	std::string Entity::GetName() const {
		return name;
	}

	void Entity::SetName(const std::string& newName) {
		name = newName;
	}

	void Entity::InitializeEntity(const std::shared_ptr<Entity>& newPointerReference, const int newId) {
		pointerReference = newPointerReference;
		id = newId;
	}

	void Entity::InitializeComponent(const std::shared_ptr<Component>& componentToInitialize) const {
		componentToInitialize->SetOwner(GetPointer());
		componentToInitialize->SetPointerReference(componentToInitialize);
		OnComponentAdded(componentToInitialize);

		for (size_t j = 0, size = components.size() - 1; j < size; ++j)
			componentToInitialize->OnComponentAdded(components[j]);
	}

	std::shared_ptr<Entity> Entity::GetPointer() const {
		return pointerReference.lock();
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