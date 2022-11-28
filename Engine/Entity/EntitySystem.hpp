#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Utility/Event.hpp"

namespace Engine {
	class Component;
	class Entity;
	class Scene;

	/// <summary>
	/// This object keeps track of all available entities. NOTE: only the Engine class is allowed to create this object.
	/// </summary>
	class ENGINE_API EntitySystem {
		friend std::shared_ptr<EntitySystem> Engine::GetEntitySystem() noexcept;
		friend class Application;

		std::vector<std::shared_ptr<Entity>> system;
		std::shared_ptr<SceneManager> sceneManager;

		EntitySystem();
	public:
		Sharp::Event<std::shared_ptr<Entity>> OnEntityAddedEvent;
		Sharp::Event<std::shared_ptr<Entity>> OnEntityRemovedEvent;
		Sharp::Event<std::shared_ptr<Entity>, std::shared_ptr<Component>> OnComponentAddedToEntityEvent;
		Sharp::Event<std::shared_ptr<Entity>, std::shared_ptr<Component>> OnComponentRemovedFromEntityEvent;

		~EntitySystem();
		EntitySystem(const EntitySystem& other) = delete;
		EntitySystem(EntitySystem&& other) noexcept = delete;

		EntitySystem& operator=(const EntitySystem& other) = delete;
		EntitySystem& operator=(EntitySystem&& other) noexcept = delete;

		static std::shared_ptr<EntitySystem> Get();

		/// <summary>
		/// This method allows you to clear the entire entity system.
		/// </summary>
		void RemoveAllEntities();

		/// <summary>Allows you to create a new Entity.</summary>
		/// <param name="entityName">The name of the entity you want to create, this is empty by default.</param>
		/// <returns>Returns the newly created entity as a shared pointer.</returns>
		[[nodiscard]] std::shared_ptr<Entity> CreateEntity(const std::string& entityName = "");

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
		[[nodiscard]] std::vector<std::shared_ptr<Entity>> GetAllEntitiesWithComponent();

	private:
		void AddEntity(const std::shared_ptr<Entity>& entityToAdd);

		void RemoveEntity(std::shared_ptr<Entity> entityToRemove);

		void Update() const;

		void HandleOnSceneOpenedEvent(std::shared_ptr<Scene> openedScene);

		void HandleOnSceneClosedEvent(std::shared_ptr<Scene> closedScene);

		void HandleOnSceneActiveStateChangedEvent(std::shared_ptr<Scene> scene, bool isActive);

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
	std::vector<std::shared_ptr<Entity>> EntitySystem::GetAllEntitiesWithComponent() {
		std::vector<std::shared_ptr<Entity>> entitiesToReturn = std::vector<std::shared_ptr<Entity>>();

		for (std::shared_ptr<Entity> entity : system) {
			if (entity->GetComponent<ComponentType>())
				entitiesToReturn.push_back(entity);
		}

		return entitiesToReturn;
	}
} // namespace Engine