#include "Engine/Camera/Frustum.hpp"

#include <glm/gtc/quaternion.hpp>

namespace Engine {
	Frustum::Frustum(float fieldOfVision, float aspectRatio, glm::vec3 position, glm::quat rotation, glm::vec2 clippingPlanes, glm::vec3 right, glm::vec3 up) {
		glm::vec3 directionEuler = glm::eulerAngles(rotation);

		float tangentHalfFov = 2 * tan(fieldOfVision * 0.5f);
		float nearPlaneHeight = tangentHalfFov * clippingPlanes.x;
		float nearPlaneWidth = nearPlaneHeight * aspectRatio;

		float farPlaneHeight = tangentHalfFov * clippingPlanes.y;
		float farPlaneWidth = farPlaneHeight * aspectRatio;
		float halfFarPlaneHeight = farPlaneHeight * 0.5f;
		float halfFarPlaneWidth = farPlaneWidth * 0.5f;
		float halfNearPlaneHeight = nearPlaneHeight * 0.5f;
		float halfNearPlaneWidth = nearPlaneWidth * 0.5f;

		glm::vec3 farCenter = position + directionEuler * clippingPlanes.y;
		glm::vec3 halfFarUp = up * halfFarPlaneHeight;
		glm::vec3 halfFarRight = right * halfFarPlaneWidth;
		glm::vec3 halfNearUp = up * halfNearPlaneHeight;
		glm::vec3 halfNearRight = right * halfNearPlaneWidth;

		glm::vec3 farTopLeft = farCenter + halfFarUp - halfFarRight;
		glm::vec3 farTopRight = farCenter + halfFarUp + halfFarRight;
		glm::vec3 farBottomLeft = farCenter - halfFarUp - halfFarRight;
		glm::vec3 farBottomRight = farCenter - halfFarUp + halfFarRight;

		glm::vec3 nearCenter = position + directionEuler * clippingPlanes.x;

		glm::vec3 nearTopLeft = nearCenter + halfNearUp - halfNearRight;
		glm::vec3 nearTopRight = nearCenter + halfNearUp + halfNearRight;
		glm::vec3 nearBottomLeft = nearCenter - halfNearUp - halfNearRight;
		glm::vec3 nearBottomRight = nearCenter - halfNearUp + halfNearRight;

		topFace = Face(nearTopRight, nearTopLeft, farTopLeft);
		bottomFace = Face(nearBottomLeft, nearBottomRight, farBottomLeft);
		leftFace = Face(nearTopLeft, nearBottomLeft, farBottomLeft);
		rightFace = Face(nearBottomRight, nearTopRight, farBottomRight);
		frontFace = Face(nearTopLeft, nearTopRight, nearBottomRight);
		backFace = Face(farTopRight, farTopLeft, farBottomLeft);
	}

	Frustum::Face::Face(glm::vec3 position0, glm::vec3 position1, glm::vec3 position2) : position0(position0), position1(position1), position2(position2) {
		v = position1 - position0;
		u = position2 - position0;
		normal = v * u;
		glm::normalize(normal);
	}

	Frustum::Face Frustum::GetFrontFace() const {
		return frontFace;
	}

	Frustum::Face Frustum::GetBackFace() const {
		return backFace;
	}

	Frustum::Face Frustum::GetTopFace() const {
		return topFace;
	}

	Frustum::Face Frustum::GetRightFace() const {
		return rightFace;
	}

	Frustum::Face Frustum::GetBottomFace() const {
		return bottomFace;
	}

	Frustum::Face Frustum::GetLeftFace() const {
		return leftFace;
	}
}