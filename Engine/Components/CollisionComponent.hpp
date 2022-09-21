#pragma once

#include "Engine/Components/Component.hpp"
#include "Engine/Collision/CollisionTypes.hpp"

#include <glm/vec2.hpp>

namespace Engine
{
	class TransformComponent;

	/// <summary>
	/// The Collision component is an entities link to the Collision System and allows for interaction with the other collision components.
	/// IMPORTANT: The Collision Component requires a Transform Component to be attached to the same entitiy.
	/// </summary>
	class ENGINE_API CollisionComponent : public Component
	{
		// Shortcuts
		typedef std::shared_ptr<CollisionComponent> CollPtr;
		friend class CollisionSystem;

		CollisionComponent();
		CollisionComponent(CollisionComponent& collisionComponent);

		virtual void OnComponentAdded(std::shared_ptr<Component> addedComponent) override;

		virtual void OnComponentRemoved(std::shared_ptr<Component> removedComponent) override;

		std::weak_ptr<TransformComponent> GetTransformComponent() const;

		public:

		// Shapes
		/// <summary>
		/// Sets the Shape of the component to Circle
		/// </summary>
		/// <param name="radius">The Radius for the circle shape</param>
		void SetCircle(const float radius);

		/// <summary>
		/// Get the Radius of the circle shape (still returns even if it isn't a circle shape)
		/// </summary>
		/// <returns>float that represenets the radius of the circle shape</returns>
		float GetCircleRadius() const;

		/// <summary>
		/// Sets the Shape of the component to AABB
		/// </summary>
		/// <param name="sizeX">The size of the AABB along the x axis</param>
		/// <param name="sizeY">The size of the AABB along the y axis</param>
		void SetAABB(const float sizeX, const float sizeY);

		/// <summary>
		/// Sets the Shape of the component to AABB
		/// </summary>
		/// <param name="size">The size of the AABB</param>
		void SetAABB(const glm::vec2& size);

		/// <summary>
		/// Gets the size of AABB. (Still returns the size even if the shape isn't AABB)
		/// </summary>
		/// <returns>glm::vec2 representing the size of the AABB</returns>
		glm::vec2 GetAABBSize() const;

		/// <summary>
		/// Gets the collision shape type of the collision component
		/// </summary>
		/// <returns></returns>
		CollisionShapeType GetCollisionShapeType() const;

		// Layers
		/// <summary>
		/// Sets the collision layer
		/// The collision layer is the "type" of the collision body,other collision bodies looking for this "type" will generate a collision response if they are detectable by this collision body
		/// </summary>
		/// <param name="layer">The new layer</param>
		void SetCollisionLayer(CollisionLayer layer);

		/// <summary>
		/// Gets the collision layer
		/// </summary>
		/// <returns>CollisionLayer representing the collision layer</returns>
		CollisionLayer GetCollisionLayer() const;

		/// <summary>
		/// Sets the detection layers
		/// The detection layers are what this collision object will be looking for to collide/interact with.
		/// </summary>
		/// <param name="layers">The new detection layers</param>
		void SetDetectionLayers(CollisionLayer layers);

		/// <summary>
		/// Gets the detection layers
		/// </summary>
		/// <returns>CollisionLayer representing the detection layers</returns>
		CollisionLayer GetDetectionLayers() const;

		/// <summary>
		/// Sets if the collision object will be a sensor or not.
		/// NOTE: A sensor is an object that does do collision callbacks but does not do collision correction/response on world update.
		/// NOTE: Use this if you want something to act as a trigger.
		/// </summary>
		/// <param name="isSensor">The new value determining if the collision object will be a sensor.</param>
		void SetIsSensor(bool isSensor);

		/// <summary>
		/// Returns true if the collision object is a sensor
		/// </summary>
		/// <returns>A bool representing whether the collision object is a sensor or not</returns>
		bool GetIsSensor() const;
			/// <summary>
		/// Saves the data of this component to a archive 
		/// </summary>
		/// <param name="ar"> the archive that you want to save to </param>
		template <typename archive>
		void SaveComponent(archive ar);

		/// <summary>
		/// loads the data from the archive to this component
		/// </summary>
		/// <param name="ar"> the archive that you want to load from </param>
		template<typename archive>
		void LoadComponent(archive ar);
	private:
		CollisionShapeType shapeType;			/// The Shape Type, AABB or Circle
		std::weak_ptr<TransformComponent> transformComponent;
	};
}
