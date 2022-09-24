#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Engine/Engine.hpp"

#include <memory>
#include <utility>

namespace Engine {
	int numberOfInstantiatedEntities = 0;

	std::shared_ptr<EntitySystem> EntitySystem::Get() {
		return Engine::GetEntitySystem();
	}

	void EntitySystem::RemoveAllEntities() {
		system.clear();
	}

	std::shared_ptr<Entity> EntitySystem::CreateEntity(const std::string& entityName) {
		AddEntity(std::shared_ptr<Entity>(new Entity(entityName)));

		return system.back();
	}

	std::shared_ptr<Entity> EntitySystem::GetEntity(const int id) const {
		for (const std::shared_ptr<Entity>& entity : system) {
			if (entity->GetId() == id)
				return entity;
		}

		return {};
	}

	std::vector<std::shared_ptr<Entity>> EntitySystem::GetAllEntities() const {
		return system;
	}

	void EntitySystem::AddEntity(std::shared_ptr<Entity> entityToAdd) {
		if (entityToAdd->GetId() == -1)
			entityToAdd->InitializeEntity(entityToAdd, numberOfInstantiatedEntities++);

		entityToAdd->OnComponentAddedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentAddedToEntityEvent);
		entityToAdd->OnComponentRemovedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentRemovedFromEntityEvent);
		entityToAdd->OnEntityDestroyedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::RemoveEntity);

		OnEntityAddedEvent(entityToAdd);
		system.push_back(std::move(entityToAdd));
	}

	void EntitySystem::RemoveEntity(std::shared_ptr<Entity> entityToRemove) {
		if (system.empty()) return;

		const std::vector<std::shared_ptr<Entity>>::iterator it = std::find(system.begin(), system.end(), entityToRemove);
		if (it != system.end()) {
			entityToRemove->OnComponentAddedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentAddedToEntityEvent);
			entityToRemove->OnComponentRemovedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentRemovedFromEntityEvent);
			entityToRemove->OnEntityDestroyedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::RemoveEntity);
			it->reset();
			system.erase(it);
		}
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
} // namespace Engine