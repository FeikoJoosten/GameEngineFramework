#pragma once

#include "Engine/Entity/Entity.hpp"

#include <ThirdParty/Box2D/Box2D/Box2D.h>

namespace Engine
{
	class CollisionSystem;

	class EntityContact : public b2ContactListener
	{
		void BeginContact(b2Contact* contact) override;

		void EndContact(b2Contact* contact) override;

		void GetEntities(b2Contact* contact);

		eastl::weak_ptr<Entity> entityA_;
		eastl::weak_ptr<Entity> entityB_;
	};
} // namespace Engine