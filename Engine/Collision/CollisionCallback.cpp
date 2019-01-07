#include "Engine/Collision/CollisionCallback.hpp"

#include "Engine/engine.hpp"

namespace Engine {

	void EntityContact::BeginContact(b2Contact* contact)
	{
		GetEntities(contact);

		entityA_.lock()->OnBeginContact(entityB_);
		entityB_.lock()->OnBeginContact(entityA_);
	}

	void EntityContact::EndContact(b2Contact* contact)
	{
		GetEntities(contact);

		entityA_.lock()->OnEndContact(entityB_);
		entityB_.lock()->OnEndContact(entityA_);
	}

	void EntityContact::GetEntities(b2Contact* contact)
	{
		eastl::weak_ptr<EntitySystem> entitySystem = Engine::GetEngine().lock()->GetEntitySystem();

		const uint64_t entityIdA = uint64_t(contact->GetFixtureA()->GetUserData());
		const uint64_t entityIdB = uint64_t(contact->GetFixtureB()->GetUserData());

		entityA_ = entitySystem.lock()->GetEntity(entityIdA);
		entityB_ = entitySystem.lock()->GetEntity(entityIdB);
	}
} // namespace Engine