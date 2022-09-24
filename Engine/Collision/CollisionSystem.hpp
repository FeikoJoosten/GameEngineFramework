#pragma once

#include "Engine/Collision/CollisionTypes.hpp"
#include "Engine/Components/CollisionComponent.hpp"
#include "Engine/Api/Api.hpp"

#include <map>
#include <vector>
#include <glm/vec2.hpp>


namespace Engine
{
	class EntitySystem;

	/// <summary>
	/// The Collision System handles and stores run-time collision data using the Box2D library
	/// </summary>
	class ENGINE_API CollisionSystem
	{
		friend class Engine;
		
		CollisionSystem();
	public:
		~CollisionSystem();
	private:

		friend class CollisionComponent;
		void AddCollisionComponent(const std::weak_ptr<CollisionComponent>& componentToAdd);
		void RemoveCollisionComponent(const std::weak_ptr<CollisionComponent>& componentToRemove);

		void OnLevelLoaded();
		void OnLevelUnloaded();
	public:
	
		/// <summary>
		/// Stops the Collision System, emptying m_collisionComponents
		/// Prevents further calls to Update().
		/// </summary>
		void Stop();

		/// <summary>
		/// Starts the Collision System, gathering all required component data and generating collision bodies from it.
		/// Allows further calls to Update().
		/// </summary>
		void Start();

		/// <summary>
		/// Updates the collision world.
		/// Takes transform data of collision components, changes it, and replaces the old transform data with the new data.
		/// </summary>
		void Update();

		/// <summary>
		/// Returns the first collision-body hit by the ray. TODO - Implementation
		/// </summary>
		/// <param name="start">The start position of the ray.</param>
		/// <param name="end">The end position of the ray.</param>
		/// <param name="detectionLayers">The collision layers you want to use.</param>
		/// <returns>shared_ptr of CollisionComponent of the first collision-body hit by the ray.</returns>
		std::weak_ptr<Entity>	RayQueryFirstHit(const glm::vec2& start, const glm::vec2& end, CollisionLayer detectionLayers) const;

		/// <summary>
		/// Returns all collision-objects hit by the ray. TODO - Implementation
		/// </summary>
		/// <param name="start">The start position of the ray.</param>
		/// <param name="end">The end position of the ray.</param>
		/// <returns>vector of shared_ptr of CollisionComponent of all collision-bodies hit by the ray</returns>
		std::weak_ptr<Entity>	RayQueryAllHit(const glm::vec2& start, const glm::vec2& end) const; // TODO - Implementation

		/// <summary>
		/// Returns true if the Collision system is running
		/// </summary>
		/// <returns>bool that represents wheter or not the collision system is running.</returns>
		bool IsRunning() const;

		/// <summary>
		/// Gets all of the collision components that are currently active in the world.
		/// </summary>
		/// <returns>std::vector of weak pointers of collision components</returns>
		std::vector<std::weak_ptr<CollisionComponent>>	GetActiveCollisionComponents() const;

	private:
		/// <summary>
		/// Clears all the collision-bodies from the collision-world
		/// </summary>
		void ClearWorld();

		void HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent);
		void HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent);

		std::weak_ptr<EntitySystem> entitySystem;
		std::vector<std::weak_ptr<CollisionComponent>> collisionComponents;	/// All the components related to the currently active collision-bodies in m_world
	};
} // namespace Engine
