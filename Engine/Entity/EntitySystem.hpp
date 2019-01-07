#pragma once
#include "Engine/api.hpp"
#include "Engine/Entity/Entity.hpp"

#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>
#include <ThirdParty/EASTL-master/include/EASTL/string.h>

namespace Engine
{
	/// <summary>
	/// This object keeps track of all available entities. NOTE: only the Engine class is allowed to create this object.
	/// </summary>
	class ENGINE_API EntitySystem
	{
	public:
		/// <summary>
		/// This method allows you to clear the entire entity system and all the teams.
		/// </summary>
		void RemoveAllEntities();

		/// <summary>
		/// Allows you to create a new Entity. This method will also add the entity to the given team if that team id exists.
		/// If the given team id does not exist, or it's kept on 0, it'll add the entity to the last created team.
		/// If there are no excisting teams, it'll create a new team and add the entity to it.
		/// Use teamId -1 to not add a an entity to a team at all.
		/// NOTE: Using a teamId lower than -1 will revert the teamId to -1.
		/// </summary>
		/// <param name="entityName">The name of the entity you want to create, this is empty by default.</param>
		/// <param name="teamId">The team id you want to add the entity to. Default is 0</param>
		/// <returns>Returns the newly created entity as a weak pointer.</returns>
		eastl::weak_ptr<Entity> CreateEntity(eastl::string entityName = "", int teamId = 0);

		template<typename EntityType>
		/// <summary>
		/// Allows you to create a new Entity. This method will also add the entity to the given team if that team id exists.
		/// If the given team id does not exist, or it's kept on 0, it'll add the entity to the last created team.
		/// If there are no excisting teams, it'll create a new team and add the entity to it.
		/// Use teamId -1 to not add a an entity to a team at all.
		/// NOTE: Using a teamId lower than -1 will revert the teamId to -1.
		/// </summary>
		/// <param name="entityName">The name of the entity you want to create, this is empty by default.</param>
		/// <returns>Returns the newly created entity as a weak pointer.</returns>
		eastl::weak_ptr<EntityType> CreateEntity(eastl::string entityName = "");

		template<class EntityType, class... Args>
		/// <summary>
		/// Allows you to create a new Entity. This method will also add the entity to the given team if that team id exists.
		/// If the given team id does not exist, or it's kept on 0, it'll add the entity to the last created team.
		/// If there are no excisting teams, it'll create a new team and add the entity to it.
		/// Use teamId -1 to not add a an entity to a team at all.
		/// NOTE: Using a teamId lower than -1 will revert the teamId to -1.
		/// </summary>
		/// <param name="entityName">The name of the entity you want to create, this is empty by default.</param>
		/// <param name="...args">Any given argument you require for your entity.</param>
		/// <returns>Returns the newly created entity as a weak pointer.</returns>
		eastl::weak_ptr<EntityType> CreateEntity(eastl::string entityName = "", Args&&... args);

		/// <summary>
		/// Allows you to get the entity you want based on the id number.
		/// NOTE: if you know the team id of the entity, it's often faster to call the method with the team id defined.
		/// </summary>
		/// <param name="Id">The entity you want to get.</param>
		/// <returns>Will return the entity with the given id if found. Otherwise will return an empty weak pointer.</returns>
		eastl::weak_ptr<Entity> GetEntity(uint64_t Id);

		/// <summary>
		/// This method will allow you to get all entities availible.
		/// </summary>
		/// <returns>Returns a vector of all the entities availible.</returns>
		eastl::vector<eastl::shared_ptr<Entity>> GetAllEntities() const;

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get a vector of weak pointers with the component type you are looking for.
		/// </summary>
		/// <returns>Returns a vector of weak pointers if any available entities have the components you are looking for. Otherwise returns an empty vector.</returns>
		eastl::vector<eastl::weak_ptr<ComponentType>> GetAllComponents();

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get a vector of weak pointer with entities that have the defined component attached to them.
		/// </summary>
		/// <returns>Returns a vector of weak pointers if any available entities have the component you are looking for. Otherwise returns an empty vector.</returns>
		eastl::vector<eastl::weak_ptr<Entity>> GetAllEntitiesWithComponent();

		/// <summary>
		/// This method allows you to add a precreated entity to the entity system
		/// <param name="entityToAdd">The entity you want to add to the entity system.</param>
		/// </summary>
		void AddEntity(eastl::shared_ptr<Entity> entityToAdd);

		/// <summary>
		/// This method allows you to remove a specific entity from the entity system
		/// <param name="entityToRemove">The entity you want to remove from the entity system.</param>
		/// </summary>
		void RemoveEntity(eastl::shared_ptr<Entity> entityToRemove);

	private:
		eastl::vector<eastl::shared_ptr<Entity>> system;

		friend class Engine;

		EntitySystem() = default;
	public:
		~EntitySystem();
	private:

		void Update();
	};

	template <typename EntityType>
	eastl::weak_ptr<EntityType> EntitySystem::CreateEntity(eastl::string entityName)
	{
		AddEntity(eastl::shared_ptr<EntityType>(new EntityType(entityName)));

		system.back()->InitializeEntity();

		return eastl::static_pointer_cast<EntityType, Entity>(system.back());
	}

	template <class EntityType, class... Args>
	eastl::weak_ptr<EntityType> EntitySystem::CreateEntity(eastl::string entityName, Args&&... args)
	{
		AddEntity(eastl::shared_ptr<EntityType>(new EntityType(eastl::forward<Args>(args)...)));

		system.back()->name = entityName;
		system.back()->InitializeEntity();

		return eastl::static_pointer_cast<EntityType, Entity>(system.back());
	}

	template <typename ComponentType>
	eastl::vector<eastl::weak_ptr<ComponentType>> EntitySystem::GetAllComponents()
	{
		eastl::vector<eastl::weak_ptr<ComponentType>> componentsToReturn = eastl::vector<eastl::weak_ptr<ComponentType>>();

		for (eastl::vector<eastl::shared_ptr<Entity>>::iterator it = system.begin(), end = system.end(); it != end; ++it)
		{
			eastl::vector<eastl::weak_ptr<ComponentType>> componentsToAdd = it->get()->GetComponents<ComponentType>();

			componentsToReturn.reserve(componentsToReturn.size() + componentsToAdd.size());
			componentsToReturn.insert(componentsToReturn.end(), componentsToAdd.begin(), componentsToAdd.end());
		}

		return componentsToReturn;
	}

	template <typename ComponentType>
	eastl::vector<eastl::weak_ptr<Entity>> EntitySystem::GetAllEntitiesWithComponent()
	{
		eastl::vector<eastl::weak_ptr<Entity>> entitiesToReturn = eastl::vector<eastl::weak_ptr<Entity>>();

		for (size_t i = 0, size = system.size(); i < size; ++i)
		{
			if (system[i].get()->GetComponent<ComponentType>().expired() != false)
				entitiesToReturn.push_back(system[i]);
		}

		return entitiesToReturn;
	}
} // namespace Engine