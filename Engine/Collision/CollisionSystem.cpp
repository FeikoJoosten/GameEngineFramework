#include "Engine/Collision/CollisionSystem.hpp"

#include "Engine/engine.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Collision/CollisionCallback.hpp"

// To cast the int to void* without getting compiler warnings
#define INT_TO_VOID_PTR(val) ((void*)(size_t) val)

namespace Engine
{

	CollisionSystem::CollisionSystem() : world_(b2Vec2(0, 0)), isRunning_(false)
	{
		world_.SetAllowSleeping(false);
	}

	CollisionSystem::~CollisionSystem()
	{
	}

	void CollisionSystem::AddCollisionComponent(eastl::weak_ptr<CollisionComponent> componentToAdd)
	{
		collisionComponents_.push_back(componentToAdd);

		// Generate a Collision Body
		componentToAdd.lock()->body = world_.CreateBody(&(componentToAdd.lock()->bodyDef));
		componentToAdd.lock()->body->CreateFixture(&(componentToAdd.lock()->fixtureDef));
		componentToAdd.lock()->body->GetFixtureList()->SetUserData(INT_TO_VOID_PTR(componentToAdd.lock()->GetOwner().lock()->GetID()));
	}

	void CollisionSystem::RemoveCollisionComponent(CollisionComponent*  componentToRemove)
	{
		if (world_.GetBodyCount() == 0)
			return;
		if (componentToRemove == nullptr)
			return;
		if (componentToRemove->body == nullptr)
			return;

		world_.DestroyBody(componentToRemove->body);
	}

	void CollisionSystem::OnLevelLoaded()
	{
		if (collisionComponents_.size() > 0)
			Start();
	}

	void CollisionSystem::OnLevelUnloaded()
	{
		if (isRunning_ == false)
			return;

		Stop();
		collisionComponents_.clear();
	}

	void CollisionSystem::Stop()
	{
		ClearWorld();
		isRunning_ = false;
	}

	void CollisionSystem::Start()
	{
		ClearWorld();
		isRunning_ = true;
	}

	void CollisionSystem::Update()
	{
		// m_isRunning = true; // Use this to remove the assert if you're not working with the collision system, just don't submit it
		if (isRunning_ == false) 
			return; // Call CollisionSystem::Start() before calling CollisionSystem::Update()!

		world_.SetContactListener(&entityContactCallback_);

		// Update World Positions with Transform Components positions
		for (auto colComp : collisionComponents_)
		{
			eastl::weak_ptr<TransformComponent> transform = colComp.lock()->GetTransformComponent();
			if (transform.expired() == false) // Collision Component Requires Transform!
				continue;

			const glm::vec3 position = transform.lock()->GetPosition();
			colComp.lock()->body->SetTransform(b2Vec2(position.x, position.y), colComp.lock()->body->GetAngle());
		}

		// Update World
		const float deltaTime = Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime();
		world_.Step(deltaTime, 8, 4);

		// Update Transform Positions with World Positions
		for (auto colComp : collisionComponents_)
		{
			eastl::weak_ptr<TransformComponent> transform = colComp.lock()->GetTransformComponent();
			if (transform.expired() == false) // Collision Component Requires Transform!
				continue;

			const glm::vec3 oldPosition = transform.lock()->GetPosition();
			const b2Vec2& position = colComp.lock()->body->GetPosition();

			transform.lock()->SetPosition(glm::vec3(position.x, position.y, oldPosition.z));
		}
		entityContactCallback_ = EntityContact();
	}

	eastl::weak_ptr<Entity> CollisionSystem::RayQueryFirstHit(const glm::vec2& start, const glm::vec2& end, CollisionLayer detectionLayers) const
	{
		RayCastClosestCallback callback;
		callback.detectionLayers = detectionLayers;

		// Raycast of 0 distance causes assert error in Box2D
		if(start == end)
		{
			return eastl::weak_ptr<Entity>();
		}

		world_.RayCast(&callback, b2Vec2(start.x, start.y), b2Vec2(end.x, end.y));

		return callback.entity;
	}

	// TODO - Implementation
	eastl::weak_ptr<Entity> CollisionSystem::RayQueryAllHit(const glm::vec2& start, const glm::vec2& end) const
	{
		return eastl::weak_ptr<Entity>();
	}

	bool CollisionSystem::IsRunning() const
	{
		return isRunning_;
	}

	eastl::vector<eastl::weak_ptr<CollisionComponent>> CollisionSystem::GetActiveCollisionComponents() const
	{
		return collisionComponents_;
	}

	void CollisionSystem::ClearWorld()
	{
		b2Body* body = world_.GetBodyList();

		while (body != nullptr)
		{
			b2Body* nextBody = body->GetNext();
			world_.DestroyBody(body);
			body = nextBody;
		}
	}

}
