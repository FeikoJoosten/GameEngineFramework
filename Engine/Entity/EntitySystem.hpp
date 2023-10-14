#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Utility/Event.hpp"

namespace Engine {
	class Component;

	/// <summary>
	/// This object keeps track of all available entities. NOTE: only the Engine class is allowed to create this object.
	/// </summary>
	class ENGINE_API EntitySystem {
		friend std::shared_ptr<EntitySystem> Engine::GetEntitySystem() noexcept;
		friend class Application;

		std::vector<std::shared_ptr<Entity>> system;

		explicit EntitySystem() = default;
	public:
		Sharp::Event<std::shared_ptr<Entity>> OnEntityAddedEvent;
		Sharp::Event<std::shared_ptr<Entity>> OnEntityRemovedEvent;
		Sharp::Event<std::shared_ptr<Entity>, std::shared_ptr<Component>> OnComponentAddedToEntityEvent;
		Sharp::Event<std::shared_ptr<Entity>, std::shared_ptr<Component>> OnComponentRemovedFromEntityEvent;

		~EntitySystem() = default;
		EntitySystem(const EntitySystem& other) = delete;
		EntitySystem(EntitySystem&& other) noexcept = delete;

		EntitySystem& operator=(const EntitySystem& other) = delete;
		EntitySystem& operator=(EntitySystem&& other) noexcept = delete;

		static std::shared_ptr<EntitySystem> Get();

		/// <summary>
		/// This method allows you to clear the entire entity system.
		/// </summary>
		void RemoveAllEntities();

		/// <summary>
		/// This method will allow you to get all entities available.
		/// </summary>
		/// <returns>Returns a vector of all the entities available.</returns>
		[[nodiscard]] std::vector<std::shared_ptr<Entity>> GetAllEntities() const;

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get a vector of shared pointers with the component type you are looking for.
		/// </summary>
		/// <returns>Returns a vector of shared pointers if any available entities have the components you are looking for. Otherwise returns an empty vector.</returns>
		[[nodiscard]] std::vector<std::shared_ptr<ComponentType>> GetAllComponents();

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get a vector of shared pointer with entities that have the defined component attached to them.
		/// </summary>
		/// <returns>Returns a vector of shared pointers if any available entities have the component you are looking for. Otherwise returns an empty vector.</returns>
		[[nodiscard]] std::vector<std::shared_ptr<Entity>> GetAllEntitiesWithComponent() const;

		ENGINE_LOCAL void AddEntity(const std::shared_ptr<Entity>& entityToAdd);

		ENGINE_LOCAL void RemoveEntity(const std::shared_ptr<Entity>& entityToRemove);

	private:
		
		void Update() const;

		void HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent);

		void HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent);
	};

	template <typename ComponentType>
	std::vector<std::shared_ptr<ComponentType>> EntitySystem::GetAllComponents() {
		std::vector<std::shared_ptr<ComponentType>> componentsToReturn = std::vector<std::shared_ptr<ComponentType>>();

		for(std::shared_ptr<Entity> entity : system) {
			std::vector<std::shared_ptr<ComponentType>> componentsToAdd = entity->GetComponents<ComponentType>();

			componentsToReturn.reserve(componentsToReturn.size() + componentsToAdd.size());
			componentsToReturn.insert(componentsToReturn.end(), componentsToAdd.begin(), componentsToAdd.end());
		}

		return componentsToReturn;
	}

	template <typename ComponentType>
	std::vector<std::shared_ptr<Entity>> EntitySystem::GetAllEntitiesWithComponent() const {
		std::vector<std::shared_ptr<Entity>> entitiesToReturn = std::vector<std::shared_ptr<Entity>>();

		for (std::shared_ptr<Entity> entity : system) {
			if (entity->GetComponent<ComponentType>())
				entitiesToReturn.push_back(entity);
		}

		return entitiesToReturn;
	}
}