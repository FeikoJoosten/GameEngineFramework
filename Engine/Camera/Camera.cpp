#include "Engine/Camera/Camera.hpp"
#include "Engine/Utility/defines.hpp"
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>

namespace Engine
{
	Camera::Camera(glm::vec3 position, glm::vec3 rotation, float fov, float aspectRatio, float zNear, float zFar) :
		fov(fov), aspectRatio(aspectRatio), verticalAngle(0), clippingPlanes(zNear, zFar), position(position),
		rotation(rotation)
	{
		glm::vec3 target = glm::normalize(position + rotation);
		view = glm::lookAt(position, position - target, glm::normalize(GetUp()));
		projection = glm::perspective(glm::radians(fov), aspectRatio, clippingPlanes.x, clippingPlanes.y);
	}

	float Camera::GetFoV() const
	{
		return fov;
	}

	float Camera::GetAspectRatio() const
	{
		return aspectRatio;
	}

	glm::vec2 Camera::GetClippingPlanes() const
	{
		return clippingPlanes;
	}

	glm::vec3 Camera::GetUp()
	{
#if RENDERER == 1
		return glm::cross(GetRight(), rotation);
#elif RENDERER == 2
		return -glm::cross(GetRight(), rotation);
#else
		return glm::cross(GetRight(), rotation);
#endif
	}

	glm::vec3 Camera::GetRight()
	{
		return glm::vec3(
			sin(horizontalAngle - glm::half_pi<float>()),
			0,
			cos(horizontalAngle - glm::half_pi<float>()));
	}

	glm::vec3 Camera::GetPosition()
	{
		return position;
	}

	glm::vec3 Camera::GetRotation()
	{
		return rotation;
	}

	glm::mat4x4 Camera::GetView() const
	{
		return view;
	}

	glm::mat4x4 Camera::GetProjection() const
	{
		return projection;
	}

	glm::mat4x4 Camera::GetViewProjection() const
	{
		return view * projection;
	}

	Frustum Camera::GetFrustum()
	{
		return frustum;
	}

	void Camera::SetProjection(float fov, float aspectRatio, float zNear, float zFar)
	{
		this->fov = fov;
		this->aspectRatio = aspectRatio;
		clippingPlanes.x = zNear;
		clippingPlanes.y = zFar;

		projection = glm::perspective(fov, aspectRatio, zNear, zFar);
	}

	void Camera::SetView(glm::mat4x4 view)
	{
		this->view = view;
	}

	void Camera::SetViewProjection(glm::mat4x4 view, float fov, float aspectRatio, float zNear, float zFar)
	{
		SetView(view);
		SetProjection(fov, aspectRatio, zNear, zFar);
	}

	void Camera::UpdateFrustum(glm::vec3 right, glm::vec3 up)
	{
		frustum = Frustum(*this, right, up);
	}

	void Camera::UpdateRotationOffset(glm::vec2 rotationOffset)
	{
		horizontalAngle += rotationOffset.x;
		verticalAngle += rotationOffset.y;

		rotation = glm::vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle));
	}

	void Camera::MoveLeft(float distanceToMove)
	{
		position += -glm::vec3(distanceToMove, 0, 0);
	}

	void Camera::MoveRight(float distanceToMove)
	{
		position += glm::vec3(distanceToMove, 0, 0);
	}

	void Camera::MoveForwards(float distanceToMove)
	{
		position += glm::vec3(0, 0, distanceToMove);
	}

	void Camera::MoveBackwards(float distanceToMove)
	{
		position += -glm::vec3(0, 0, distanceToMove);
	}
} // namespace Engine
