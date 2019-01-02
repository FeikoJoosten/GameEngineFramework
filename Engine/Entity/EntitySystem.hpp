#pragma once
#include "Engine/api.hpp"

#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	class Entity;

	class ENGINE_API EntitySystem
	{
	public:
		EntitySystem();
		~EntitySystem();

	private:
		eastl::vector<Entity*> system;

		friend class Engine;
		void Update();

		void Render();

		void AddEntity(Entity* entityToAdd);
		void RemoveEntity(Entity* entityToRemove);
	};
} // namespace Engine