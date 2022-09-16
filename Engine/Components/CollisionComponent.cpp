#include "Engine/Components/CollisionComponent.hpp"

#include "Engine/Engine.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Collision/CollisionSystem.hpp"
#include "Engine/Components/TransformComponent.hpp"

#include <cereal/archives/json.hpp>

namespace Engine
{
	void CollisionComponent::OnComponentAdded(std::shared_ptr<Component> addedComponent)
	{
		if (std::dynamic_pointer_cast<TransformComponent>(addedComponent) && transformComponent.expired())
		{
			transformComponent = std::static_pointer_cast<TransformComponent>(addedComponent);
		}
	}

	void CollisionComponent::OnComponentRemoved(std::shared_ptr<Component> removedComponent)
	{
		if (transformComponent.lock().get() == removedComponent.get())
			transformComponent.reset();

		//In case we have multiple transformcomponents on this entity for some reason.
		transformComponent = GetComponent<TransformComponent>();
	}

	std::weak_ptr<TransformComponent> CollisionComponent::GetTransformComponent() const
	{
		return transformComponent;
	}

	CollisionComponent::CollisionComponent()
	{
	}

	CollisionComponent::CollisionComponent(CollisionComponent& collisionComponent)
	{
		shapeType		= collisionComponent.shapeType;
	}

	void CollisionComponent::SetCircle(const float radius)
	{
		shapeType = CollisionShapeType::Circle;
	}

	float CollisionComponent::GetCircleRadius() const
	{
		return 0;
	}

	void CollisionComponent::SetAABB(const float sizeX, const float sizeY)
	{
		shapeType = CollisionShapeType::Box;
	}

	void CollisionComponent::SetAABB(const glm::vec2& size)
	{
		SetAABB(size.x, size.y);
	}

	glm::vec2 CollisionComponent::GetAABBSize() const
	{
		return glm::vec2(0, 0);
	}

	CollisionShapeType CollisionComponent::GetCollisionShapeType() const
	{
		return shapeType;
	}

	void CollisionComponent::SetCollisionLayer(CollisionLayer layer)
	{
	}

	CollisionLayer CollisionComponent::GetCollisionLayer() const
	{
		return CollisionLayer(0);
	}

	void CollisionComponent::SetDetectionLayers(CollisionLayer layers)
	{
	}

	CollisionLayer CollisionComponent::GetDetectionLayers() const
	{
		return CollisionLayer(0);
	}

	void CollisionComponent::SetIsSensor(bool isSensor)
	{
	}

	bool CollisionComponent::GetIsSensor() const
	{
		return false;
	}
	template <typename archive>
	void CollisionComponent::SaveComponent(archive ar)
	{
		int bodyType = static_cast<int>(this->GetBodyType());
		float circleRadius = GetCircleRadius();
		glm::vec2 aaBBsize = this->GetAABBSize();
		float aaBBsizeArr[] = { aaBBsize.x, aaBBsize.y };
		int collisionLayer = static_cast<int>(this->GetCollisionLayer());
		int detectionLayer = static_cast<int>(this->GetDetectionLayers());

		ar(CEREAL_NVP(bodyType), CEREAL_NVP(circleRadius), CEREAL_NVP(aaBBsizeArr), CEREAL_NVP(collisionLayer), CEREAL_NVP(detectionLayer), CEREAL_NVP(isEnabled));
	}


	template <typename archive>
	void CollisionComponent::LoadComponent(archive ar)
	{
		int bodyType;
		float circleRadius;
		float aaBBsizeArr[];
		int collisionLayer;
		int detectionLayer;

		ar(bodyType, circleRadius, aaBBsizeArr, collisionLayer, detectionLayer, isEnabled);

		SetBodyType(static_cast<b2BodyType>(bodyType));
		SetCircle(circleRadius);
		SetAABB(aaBBsizeArr[0], aaBBsizeArr[1]);
		SetCollisionLayer(static_cast<CollisionLayer>(collisionLayer));
		SetDetectionLayers(static_cast<CollisionLayer>(detectionLayer));
	}
}
