#include "Engine/Components/CollisionComponent.hpp"

#include "Engine/engine.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Collision/CollisionSystem.hpp"
#include "Engine/Components/TransformComponent.hpp"

#include <ThirdParty/cereal/include/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/include/cereal/archives/json.hpp>

namespace Engine
{
	CollisionComponent::CollisionComponent(CollisionShapeType shapeType, b2BodyType bodyType)
		: bodyDef()
		, fixtureDef()
		, circleShape()
		, boxShape()
		, shapeType(shapeType)
	{
		// Set the Shape
		if (shapeType == CollisionShapeType::CIRCLE)
			SetCircle(1);
		else if (shapeType == CollisionShapeType::BOX)
			SetAABB(1, 1);
		else
			assert(false); // Unregistered CollisionObjectType!

		// Set body settings
		bodyDef.fixedRotation = true;
		bodyDef.gravityScale = 0;
		bodyDef.linearDamping = 9999.0f;
		bodyDef.type = bodyType;
		// Set Fixture Settings
		fixtureDef.filter.categoryBits = 0;
		fixtureDef.filter.maskBits = uint16(CollisionLayer::SOLID);
		fixtureDef.isSensor = false;
	}

	void CollisionComponent::InitializeComponent()
	{
		Engine::GetEngine().lock()->GetCollisionSystem().lock()->AddCollisionComponent(GetPointerRefence<CollisionComponent>());
	}

	CollisionComponent::~CollisionComponent()
	{
		Engine::GetEngine().lock()->GetCollisionSystem().lock()->RemoveCollisionComponent(this);
	}

	void CollisionComponent::OnComponentAdded(eastl::weak_ptr<Component> addedComponent)
	{
		if (eastl::dynamic_pointer_cast<TransformComponent>(addedComponent.lock()) && transformComponent.expired())
		{
			transformComponent = eastl::static_pointer_cast<TransformComponent>(addedComponent.lock());
		}
	}

	void CollisionComponent::OnComponentRemoved(eastl::weak_ptr<Component> removedComponent)
	{
		if (transformComponent.lock().get() == removedComponent.lock().get())
			transformComponent.reset();

		//In case we have multiple transformcomponents on this entity for some reason.
		transformComponent = GetComponent<TransformComponent>();
	}

	eastl::weak_ptr<TransformComponent> CollisionComponent::GetTransformComponent() const
	{
		return transformComponent;
	}

	CollisionComponent::CollisionComponent() : CollisionComponent(CollisionShapeType::BOX, b2_staticBody)
	{
	}

	CollisionComponent::CollisionComponent(CollisionComponent& collisionComponent)
	{
		fixtureDef		= collisionComponent.fixtureDef;
		bodyDef		= collisionComponent.bodyDef;
		boxShape		= collisionComponent.boxShape;
		boxSize		= collisionComponent.boxSize;
		circleShape	= collisionComponent.circleShape;
		shapeType		= collisionComponent.shapeType;
		body			= nullptr;
	}

	void CollisionComponent::SetBodyType(b2BodyType bodyType)
	{
		bodyDef.type = bodyType;
	}

	b2BodyType CollisionComponent::GetBodyType() const
	{
		return bodyDef.type;
	}

	void CollisionComponent::SetCircle(const float radius)
	{
		shapeType = CollisionShapeType::CIRCLE;
		circleShape.m_radius = radius;
	}

	float CollisionComponent::GetCircleRadius() const
	{
		return circleShape.m_radius;
	}

	void CollisionComponent::SetAABB(const float sizeX, const float sizeY)
	{
		boxShape.SetAsBox(sizeX, sizeY);
		boxSize = b2Vec2(sizeX, sizeY);
		fixtureDef.shape = &boxShape;
		shapeType = CollisionShapeType::BOX;
	}

	void CollisionComponent::SetAABB(const glm::vec2& size)
	{
		SetAABB(size.x, size.y);
	}

	glm::vec2 CollisionComponent::GetAABBSize() const
	{
		return glm::vec2(boxSize.x, boxSize.y);
	}

	CollisionShapeType CollisionComponent::GetCollisionShapeType() const
	{
		return shapeType;
	}

	void CollisionComponent::SetCollisionLayer(CollisionLayer layer)
	{
		fixtureDef.filter.categoryBits = uint16(layer);
	}

	CollisionLayer CollisionComponent::GetCollisionLayer() const
	{
		return CollisionLayer(fixtureDef.filter.categoryBits);
	}

	void CollisionComponent::SetDetectionLayers(CollisionLayer layers)
	{
		fixtureDef.filter.maskBits = uint16(layers);
	}

	CollisionLayer CollisionComponent::GetDetectionLayers() const
	{
		return CollisionLayer(fixtureDef.filter.maskBits);
	}

	void CollisionComponent::SetIsSensor(bool isSensor)
	{
		fixtureDef.isSensor = isSensor;
	}

	bool CollisionComponent::GetIsSensor() const
	{
		return fixtureDef.isSensor;
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
