#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Engine.hpp"

#include <memory>

namespace Engine
{
	int entityCount = 0;

	std::shared_ptr<EntitySystem> EntitySystem::Get() {
		return Engine::GetEntitySystem();
	}

	void EntitySystem::RemoveAllEntities()
	{
		system.clear();
		entityCount = 0;
	}

	std::weak_ptr<Entity> EntitySystem::CreateEntity(std::string entityName)
	{
		//AddEntity(std::make_shared<Entity>(entityName));
		AddEntity(std::shared_ptr<Entity>(new Entity(entityName)));

		return system.back();
	}

	std::weak_ptr<Entity> EntitySystem::GetEntity(int id)
	{
		for (int i = 0; i < entityCount; i++)
		{
			if (system[i]->GetId() == id)
				return system[i];
		}

		return {};
	}

	void EntitySystem::Update() const {
		for (int i = 0; i < entityCount; ++i)
		{
			if (system[i]->GetIsActive())
				system[i]->Update();
		}
	}

	void EntitySystem::HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent) {
		OnComponentAddedToEntityEvent(entity, addedComponent);
	}

	void EntitySystem::HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent) {
		OnComponentRemovedFromEntityEvent(entity, removedComponent);
	}

	void EntitySystem::AddEntity(std::shared_ptr<Entity> entityToAdd)
	{
		if (entityToAdd->GetId() == -1)
			entityToAdd->SetId(entityCount);

		entityToAdd->OnComponentAddedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentAddedToEntityEvent);
		entityToAdd->OnComponentRemovedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentRemovedFromEntityEvent);

		OnEntityAddedEvent(entityToAdd);
		system.push_back(std::move(entityToAdd));
		entityCount++;
	}

	void EntitySystem::RemoveEntity(std::shared_ptr<Entity> entityToRemove)
	{
		if (system.empty()) return;

		std::vector<std::shared_ptr<Entity>>::iterator it = std::find(system.begin(), system.end(), entityToRemove);
		if (it != system.end())
		{
			entityToRemove->OnComponentAddedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentAddedToEntityEvent);
			entityToRemove->OnComponentRemovedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentRemovedFromEntityEvent);
			it->reset();
			system.erase(it);
		}

		system.shrink_to_fit();
		entityCount--;
	}

	std::vector<std::shared_ptr<Entity>> EntitySystem::GetAllEntities() const
	{
		return system;
	}
} // namespace Engine