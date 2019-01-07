#pragma once

#include "Engine/Entity/Entity.hpp"
#include "Engine/Collision/CollisionTypes.hpp"

#include <ThirdParty/Box2D/Box2D/Box2D.h>

namespace Engine
{
	class CollisionSystem;

	/// <summary>
	/// Gets the first entity hit by the ray
	/// Callback class used to get collision data from ray queries
	/// </summary>
	class RayCastClosestCallback : public b2RayCastCallback
	{
		friend class CollisionSystem;
	public:
		RayCastClosestCallback();
		RayCastClosestCallback(const CollisionLayer detectLayers);

		/// <summary>
		/// Function used by Box2D to tell the Callback class that the ray has collided with a fixture.
		/// Box2D Comments: By returning the current fraction, we instruct the calling code to clip the ray and
		/// continue the ray-cast to the next fixture. WARNING: do not assume that fixtures
		/// are reported in order. However, by clipping, we can always get the closest fixture.
		/// </summary>
		/// <param name="fixture">The fixture the ray intersected with</param>
		/// <param name="point">The point the ray intersected with the fixture</param>
		/// <param name="normal">The normal of the surface the ray intersected with</param>
		/// <param name="fraction">???Investigate Box2D for this???</param>
		/// <returns>Number determines the query behaviour of the ray, 1 continue, 0 stop and return fraction parameter to get the closest</returns>
		float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) override;

	private:
		eastl::weak_ptr<Entity> entity;
		bool hit;
		b2Vec2 point;
		b2Vec2 normal;
		CollisionLayer detectionLayers;
	};

	/// <summary>
	/// Gets the all entities hit by the ray
	/// Callback class used to get collision data from ray queries
	/// NOTE: Gets up to MAX_COUNT amount of entities, MAX_COUNT being defined inside of RaycastCallback.hpp
	/// </summary>
	class RayCastMultipleCallback : public b2RayCastCallback
	{
		friend class CollisionSystem;
	public:
		enum
		{
			MAX_COUNT = 5
		};

		// Structors
		RayCastMultipleCallback();
		/// <param name="detectLayers">The layers the ray will be checking for</param>
		RayCastMultipleCallback(const CollisionLayer detectLayers);

		/// <summary>
		/// Function used by Box2D to tell the Callback class that the ray has collided with a fixture.
		/// </summary>
		/// <param name="fixture">The fixture the ray intersected with</param>
		/// <param name="point">The point the ray intersected with the fixture</param>
		/// <param name="normal">The normal of the surface the ray intersected with</param>
		/// <returns>Number determines the query behaviour of the ray, 1 continue, 0 stop and return fraction parameter to get the closest</returns>
		float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32) override;

	private:
		std::weak_ptr<Entity> entities;
		b2Vec2 points[MAX_COUNT];
		b2Vec2 normals[MAX_COUNT];
		CollisionLayer detectionLayers;
		int32 count;
	};
} //namespace Engine