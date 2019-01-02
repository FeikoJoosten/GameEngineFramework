#pragma once
#include "Engine/api.hpp"
#include "Engine/Components/Component.hpp"

#include <ThirdParty/EASTL-master/include/EASTL/vector.h>
#include <ThirdParty/EASTL-master/include/EASTL/unique_ptr.h>
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>
#include <glm/detail/type_int.hpp>

namespace Engine
{
	class ENGINE_API Entity
	{
	public:

		Entity() = default;
		virtual ~Entity() = default;

		template<typename ComponentType>
		ComponentType* GetComponent();
		template<class ComponentType>
		eastl::vector<ComponentType*> GetComponents();
		eastl::vector<eastl::shared_ptr<Component>> GetAllComponents();

		void AddComponent(eastl::unique_ptr<Component> &&componentToAdd);
		void AddComponents(eastl::vector<eastl::unique_ptr<Component>>& componentsToAdd);
		void RemoveComponent(Component componentToRemove);
		void RemoveComponents(eastl::vector<Component>& componentsToRemove);
		virtual void Update();
		virtual void Render();

		size_t ComponentCount();
		int GetTeamID();
		int GetID();

	private:
		int teamID;
		glm::uint_t ID;
		friend class EntitySystem;
		eastl::vector<eastl::shared_ptr<Component>> components;

		friend class TeamEntitySystem;
		void SetTeamID(int teamID);
		void SetID(glm::uint_t ID);
	};

	template <typename ComponentType>
	ComponentType* Entity::GetComponent()
	{
		for (auto it = components.begin(), end = components.end(); it != end; ++it)
		{
			if (dynamic_cast<ComponentType*>(it->get()))
				return dynamic_cast<ComponentType*>(it->get());
		}
		return nullptr;
	}

	template <typename ComponentType>
	eastl::vector<ComponentType*> Entity::GetComponents()
	{
		eastl::vector<ComponentType*> componentsToReturn;

		for (auto it = components.begin(), end = components.end(); it != end; ++it)
		{
			if (typeid(it) == typeid(ComponentType*))
				componentsToReturn.push_back(dynamic_cast<ComponentType*>(it));
		}

		return componentsToReturn;
	}
} // namespace Engine
