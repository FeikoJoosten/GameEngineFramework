#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Utility/Event.hpp"
#include "Engine/engine.hpp"

namespace Engine
{
	size_t entityCount = 0;

	EntitySystem::EntitySystem()
	{
		Engine::GetRenderer().OnRender += Sharp::EventHandler::Bind(&EntitySystem::Render, this);
	}

	EntitySystem::~EntitySystem()
	{
		Engine::GetRenderer().OnRender -= Sharp::EventHandler::Bind(&EntitySystem::Render, this);

		for (auto it = system.begin(), end = system.end(); it != end; ++it)
		{
			delete it;
		}
	}

	void EntitySystem::Update()
	{
		for (size_t i = 0; i < entityCount; ++i)
		{
			system[i]->Update();
		}
	}

	void EntitySystem::Render()
	{
		for (size_t i = 0; i < entityCount; ++i)
		{
			system[i]->Render();
		}
	}

	void EntitySystem::AddEntity(Entity* entityToAdd)
	{
		system.push_back(entityToAdd);
		entityCount++;
	}

	void EntitySystem::RemoveEntity(Entity* entityToRemove)
	{
		if (system.size() == 0) return;

		Entity** it = eastl::find(system.begin(), system.end(), entityToRemove);
		if (it != system.end())
		{
			system.erase(it);
		}

		entityCount--;
	}
} // namespace common