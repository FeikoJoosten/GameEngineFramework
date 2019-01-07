#include "Engine/Camera/Frustum.hpp"
#include "Engine/Camera/Camera.hpp"

namespace Engine
{
	Frustum::Frustum(Camera& camera, glm::vec3 right, glm::vec3 up)
	{
		float fov = camera.GetFoV();
		float aspectRatio = camera.GetAspectRatio();
		glm::vec3 cameraPosition = camera.GetPosition();
		glm::vec3 cameraDirection = camera.GetRotation();
		glm::vec2 clippingPlanes = camera.GetClippingPlanes();

		float nearPlaneHeight = 2 * tan(fov / 2) * clippingPlanes.x;
		float nearPlaneWidth = nearPlaneHeight * aspectRatio;

		float farPlaneHeight = 2 * tan(fov / 2) * clippingPlanes.y;
		float farPlaneWidth = farPlaneHeight * aspectRatio;

		glm::vec3 farCenter = cameraPosition + cameraDirection * clippingPlanes.y;

		glm::vec3 ftl = farCenter + (up * farPlaneHeight / 2.0f) - (right * farPlaneWidth / 2.0f); // far top left
		glm::vec3 ftr = farCenter + (up * farPlaneHeight / 2.0f) + (right * farPlaneWidth / 2.0f); // far top right
		glm::vec3 fbl = farCenter - (up * farPlaneHeight / 2.0f) - (right * farPlaneWidth / 2.0f); // far bottom left
		glm::vec3 fbr = farCenter - (up * farPlaneHeight / 2.0f) + (right * farPlaneWidth / 2.0f); // far bottom right

		glm::vec3 nearCenter = cameraPosition + cameraDirection * clippingPlanes.x;

		glm::vec3 ntl = nearCenter + (up * nearPlaneHeight / 2.0f) - (right * nearPlaneWidth / 2.0f); // near top left
		glm::vec3 ntr = nearCenter + (up * nearPlaneHeight / 2.0f) + (right * nearPlaneWidth / 2.0f); // near top right
		glm::vec3 nbl = nearCenter - (up * nearPlaneHeight / 2.0f) - (right * nearPlaneWidth / 2.0f); // near bottom left
		glm::vec3 nbr = nearCenter - (up * nearPlaneHeight / 2.0f) + (right * nearPlaneWidth / 2.0f); // near bottom right

		topFace = Face(ntr, ntl, ftl);
		bottomFace = Face(nbl, nbr, fbl);
		leftFace = Face(ntl, nbl, fbl);
		rightFace = Face(nbr, ntr, fbr);
		frontFace = Face(ntl, ntr, nbr);
		backFace = Face(ftr, ftl, fbl);
	}

	Frustum::Face::Face(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
	{
		position0 = p0;
		position1 = p1;
		position2 = p2;

		v = p1 - p0;
		u = p2 - p0;
		normal = v * u;
		glm::normalize(normal);

		d = glm::dot(-normal, p0);
	}

	Frustum::Face Frustum::GetFrontFace() const
	{
		return frontFace;
	}

	Frustum::Face Frustum::GetBackFace() const
	{
		return backFace;
	}

	Frustum::Face Frustum::GetTopFace() const
	{
		return topFace;
	}

	Frustum::Face Frustum::GetRightFace() const
	{
		return rightFace;
	}

	Frustum::Face Frustum::GetBottomFace() const
	{
		return bottomFace;
	}

	Frustum::Face Frustum::GetLeftFace() const
	{
		return leftFace;
	}
} // namespace Engine