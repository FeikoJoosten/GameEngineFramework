#define GLM_FORCE_SWIZZLE 1
#define GLM_FORCE_RADIANS 1

#include "Engine/Camera/Camera.hpp"
#include "Engine/Utility/defines.hpp"
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>
#include <ThirdParty/glm/glm/gtx/euler_angles.hpp>

namespace Engine
{
	Camera::Camera(glm::vec3 position, glm::vec3 rotation, float fov, float aspectRatio, float zNear, float zFar) :
		fov(fov), aspectRatio(aspectRatio), verticalAngle(0), clippingPlanes(zNear, zFar), position(position),
		rotation(rotation)
	{
		horizontalAngle = glm::pi<float>();
		glm::vec3 target = glm::normalize(position + rotation);
		view = glm::lookAt(position, position - target, glm::normalize(GetUp()));
		projection = glm::perspective(glm::radians(fov), aspectRatio, clippingPlanes.x, clippingPlanes.y);
		UpdateFrustum(GetRight(), GetUp());
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

	glm::vec3 Camera::GetUp() const
	{
		return glm::cross(GetRight(), rotation);
	}

	glm::vec3 Camera::GetRight() const
	{
#ifdef USING_OPENGL
		return glm::vec3(
			sin(horizontalAngle - glm::half_pi<float>()),
			0,
			cos(horizontalAngle - glm::half_pi<float>()));
#endif
#ifdef USING_VULKAN
		return glm::vec3(
			sin(horizontalAngle - glm::half_pi<float>()),
			0,
			cos(horizontalAngle - glm::half_pi<float>()));
#endif
	}

	glm::vec3 Camera::GetPosition() const
	{
		return position;
	}

	glm::vec3 Camera::GetRotation() const
	{
		return rotation;
	}

	glm::mat4x4 Camera::GetView() const
	{
		glm::vec4 target(0.f, 0.f, -1.f, 1.f);
		target = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z)*target;
		return glm::lookAt(position, position + glm::vec3(target.x, target.y, target.z), glm::vec3(0.f, 1.f, 0.f));
	}

	glm::mat4x4 Camera::GetProjection() const
	{
		return projection;
	}

	glm::mat4x4 Camera::GetViewProjection() const
	{
		return GetView() * GetProjection();
	}

	Frustum Camera::GetFrustum() const
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
		glm::vec4 translation = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z) * -glm::vec4(distanceToMove, 0.f, 0.f, 0.f);
		position += glm::vec3(translation.x, translation.y, translation.z);
	}

	void Camera::MoveRight(float distanceToMove)
	{
		glm::vec4 translation = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z) * glm::vec4(distanceToMove, 0.f, 0.f, 0.f);
		position += glm::vec3(translation.x, translation.y, translation.z);
	}

	void Camera::MoveForwards(float distanceToMove)
	{
		glm::vec4 translation = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z) * -glm::vec4(0.f, 0.f, distanceToMove, 0.f);
		position += glm::vec3(translation.x, translation.y, translation.z);
	}

	void Camera::MoveBackwards(float distanceToMove)
	{
		glm::vec4 translation = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z) * glm::vec4(0.f, 0.f, distanceToMove, 0.f);
		position += glm::vec3(translation.x, translation.y, translation.z);
	}

	void Camera::SetPostionAndRotation(glm::vec3 position, glm::vec3 rotation)
	{
		this->position = position;
		this->rotation = rotation;
	}

	void Camera::SetPosition(glm::vec3 position)
	{
		SetPostionAndRotation(position, rotation);
	}

	void Camera::SetRotation(glm::vec3 rotation)
	{
		SetPostionAndRotation(position, rotation);
	}
} // namespace Engine
