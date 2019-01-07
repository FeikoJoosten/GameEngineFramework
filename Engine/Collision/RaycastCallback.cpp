#include "Engine/Collision/RaycastCallback.hpp"
#include "Engine/engine.hpp"

namespace Engine
{
	RayCastClosestCallback::RayCastClosestCallback() : hit(false), detectionLayers(CollisionLayer::SOLID)
	{
	}

	RayCastClosestCallback::RayCastClosestCallback(const CollisionLayer detectLayers) : hit(false), detectionLayers(detectLayers)
	{
	}

	float32 RayCastClosestCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
	{
		const bool isDetectable = uint16(detectionLayers) & fixture->GetFilterData().categoryBits;
		if (isDetectable == false)
			return 1.0f;

		// Record hit data
		hit = true;
		this->point = point;
		this->normal = normal;

		// Record Hit Entity
		const uint64_t entityID = uint64_t(fixture->GetUserData());
		entity = Engine::GetEngine().lock()->GetEntitySystem().lock()->GetEntity(entityID);

		// BOX2D Comment - By returning the current fraction, we instruct the calling code to clip the ray and
		// continue the ray-cast to the next fixture. WARNING: do not assume that fixtures
		// are reported in order. However, by clipping, we can always get the closest fixture.
		return fraction;
	}

	RayCastMultipleCallback::RayCastMultipleCallback() : detectionLayers(CollisionLayer::SOLID), count(0)
	{
	}

	RayCastMultipleCallback::RayCastMultipleCallback(const CollisionLayer detectLayers)	: detectionLayers(detectLayers), count(0)
	{
	}

	float32 RayCastMultipleCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32)
	{
		const bool isDetectable = uint16(detectionLayers) & fixture->GetFilterData().categoryBits;
		if (isDetectable == false)
			return 1.0f;

		b2Assert(count < MAX_COUNT);

		// Record hit data
		points[count] = point;
		normals[count] = normal;
		++count;

		if (count == MAX_COUNT)
		{
			// At this point the buffer is full.
			// By returning 0, we instruct the calling code to terminate the ray-cast.
			return 0.0f;
		}

		// By returning 1, we instruct the caller to continue without clipping the ray.
		return 1.0f;
	}
} // namespace Engine