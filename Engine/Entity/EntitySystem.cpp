#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Engine/Engine.hpp"

#include <memory>
#include <utility>

namespace Engine {

	std::shared_ptr<EntitySystem> EntitySystem::Get() {
		return Engine::GetEntitySystem();
	}

	void EntitySystem::RemoveAllEntities() {
		system.clear();
	}

	std::vector<std::shared_ptr<Entity>> EntitySystem::GetAllEntities() const {
		return system;
	}

	void EntitySystem::AddEntity(const std::shared_ptr<Entity>& entityToAdd) {
		entityToAdd->OnComponentAddedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentAddedToEntityEvent);
		entityToAdd->OnComponentRemovedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentRemovedFromEntityEvent);

		OnEntityAddedEvent(entityToAdd);
		system.push_back(entityToAdd);
		entityToAdd->OnEntityAddedToEntitySystem();

		for (const std::shared_ptr<Component>& component : entityToAdd->GetAllComponents())
			OnComponentAddedToEntityEvent(entityToAdd, component);
	}

	void EntitySystem::RemoveEntity(const std::shared_ptr<Entity>& entityToRemove) {
		if (system.empty()) return;

		const std::vector<std::shared_ptr<Entity>>::iterator entityLocation = std::ranges::find(system, entityToRemove);
		if (entityLocation == system.end()) return;

		entityToRemove->OnComponentAddedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentAddedToEntityEvent);
		entityToRemove->OnComponentRemovedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentRemovedFromEntityEvent);
		OnEntityRemovedEvent(*entityLocation);

		for (const std::shared_ptr<Component>& component : (*entityLocation)->GetAllComponents())
			OnComponentRemovedFromEntityEvent(entityToRemove, component);

		system.erase(entityLocation);
	}

	void EntitySystem::Update() const {
		for (const std::shared_ptr<Entity>& entity : system) {
			if (entity->GetIsActive())
				entity->Update();
		}
	}

	void EntitySystem::HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent) {
		OnComponentAddedToEntityEvent(std::move(entity), std::move(addedComponent));
	}

	void EntitySystem::HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent) {
		OnComponentRemovedFromEntityEvent(std::move(entity), std::move(removedComponent));
	}
}