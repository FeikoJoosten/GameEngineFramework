#include "Engine/Collision/CollisionSystem.hpp"

#include "Engine/Engine/Engine.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Entity/EntitySystem.hpp"

namespace Engine {

	CollisionSystem::CollisionSystem() {
		entitySystem = EntitySystem::Get();
		if (!entitySystem.expired())
			entitySystem.lock()->OnComponentAddedToEntityEvent += Sharp::EventHandler::Bind(this, &CollisionSystem::HandleOnComponentAddedToEntityEvent);
	}

	CollisionSystem::~CollisionSystem() {
		if (!entitySystem.expired())
			entitySystem.lock()->OnComponentAddedToEntityEvent -= Sharp::EventHandler::Bind(this, &CollisionSystem::HandleOnComponentAddedToEntityEvent);
	}

	void CollisionSystem::AddCollisionComponent(const std::weak_ptr<CollisionComponent>& componentToAdd) {
		collisionComponents.push_back(componentToAdd);
	}

	void CollisionSystem::RemoveCollisionComponent(const std::weak_ptr<CollisionComponent>& componentToRemove) {
		if (componentToRemove.expired()) return;

		collisionComponents.erase(
			std::remove_if(collisionComponents.begin(), collisionComponents.end(),
				[componentToRemove](const std::weak_ptr<CollisionComponent>& component) {
					return !component.expired() && component.lock().get() == componentToRemove.lock().get();
				}), collisionComponents.end());
	}

	void CollisionSystem::OnLevelLoaded() {
		if (!collisionComponents.empty())
			Start();
	}

	void CollisionSystem::OnLevelUnloaded() {
		Stop();
		collisionComponents.clear();
	}

	void CollisionSystem::Stop() {
		ClearWorld();
	}

	void CollisionSystem::Start() {
		ClearWorld();
	}

	void CollisionSystem::Update() {
		// Update World Positions with Transform Components positions
		for (const std::weak_ptr<CollisionComponent>& colComp : collisionComponents) {
			std::weak_ptr<TransformComponent> transform = colComp.lock()->GetTransformComponent();
			if (transform.expired() == false) // Collision Component Requires Transform!
				continue;

			const glm::vec3 position = transform.lock()->GetPosition();
		}

		// Update Transform Positions with World Positions
		for (const std::weak_ptr<CollisionComponent>& colComp : collisionComponents) {
			std::weak_ptr<TransformComponent> transform = colComp.lock()->GetTransformComponent();
			if (transform.expired() == false) // Collision Component Requires Transform!
				continue;
		}
	}

	std::weak_ptr<Entity> CollisionSystem::RayQueryFirstHit(const glm::vec2& start, const glm::vec2& end, CollisionLayer detectionLayers) const {
		// Raycast of 0 distance causes assert error in Box2D
		if (start == end) {
			return std::weak_ptr<Entity>();
		}

		return std::weak_ptr<Entity>();
	}

	// TODO - Implementation
	std::weak_ptr<Entity> CollisionSystem::RayQueryAllHit(const glm::vec2& start, const glm::vec2& end) const {
		return std::weak_ptr<Entity>();
	}

	bool CollisionSystem::IsRunning() const {
		return true;
	}

	std::vector<std::weak_ptr<CollisionComponent>> CollisionSystem::GetActiveCollisionComponents() const {
		return collisionComponents;
	}

	void CollisionSystem::ClearWorld() {}

	void CollisionSystem::HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent) {
		const std::shared_ptr<CollisionComponent> collisionComponent = std::dynamic_pointer_cast<CollisionComponent>(addedComponent);
		if (collisionComponent) AddCollisionComponent(collisionComponent);
	}

	void CollisionSystem::HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent) {
		const std::shared_ptr<CollisionComponent> collisionComponent = std::dynamic_pointer_cast<CollisionComponent>(removedComponent);
		if (collisionComponent) RemoveCollisionComponent(collisionComponent);
	}

}
