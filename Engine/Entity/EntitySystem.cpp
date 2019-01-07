#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/engine.hpp"

namespace Engine
{
	size_t entityCount = 0;

	EntitySystem::~EntitySystem()
	{
		system.clear();
	}

	void EntitySystem::RemoveAllEntities()
	{
		system.clear();
		entityCount = 0;
	}

	eastl::weak_ptr<Entity> EntitySystem::CreateEntity(eastl::string entityName, int teamId)
	{
		AddEntity(eastl::shared_ptr<Entity>(new Entity(entityName)));
		system.back()->InitializeEntity();

		return system.back();
	}

	eastl::weak_ptr<Entity> EntitySystem::GetEntity(uint64_t Id)
	{
		for (size_t i = 0; i < entityCount; i++)
		{
			if (system[i]->GetID() == Id)
				return system[i];
		}

		return eastl::weak_ptr<Entity>();
	}

	void EntitySystem::Update()
	{
		for (size_t i = 0; i < entityCount; ++i)
		{
			if (system[i]->GetIsActive())
				system[i]->Update();
		}
	}

	void EntitySystem::AddEntity(eastl::shared_ptr<Entity> entityToAdd)
	{
		if (entityToAdd->GetID() == -1)
			entityToAdd->SetID(entityCount);

		system.push_back(eastl::move(entityToAdd));
		entityCount++;
	}

	void EntitySystem::RemoveEntity(eastl::shared_ptr<Entity> entityToRemove)
	{
		if (system.size() == 0) return;

		eastl::shared_ptr<Entity>* it = eastl::find(system.begin(), system.end(), entityToRemove);
		if (it != system.end())
		{
			it->reset();
			system.erase(it);
		}

		system.shrink_to_fit();
		entityCount--;
	}

	eastl::vector<eastl::shared_ptr<Entity>> EntitySystem::GetAllEntities() const
	{
		return system;
	}
} // namespace Engine