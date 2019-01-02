#pragma once

#include "Engine/api.hpp"
#include "Engine/Camera/Frustum.hpp"
#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/glm/glm/gtc/constants.hpp>

namespace Engine
{
	class ENGINE_API Camera
	{
	public:

		Camera() = delete;
		Camera(glm::vec3 position, glm::vec3 rotation, float fov, float aspectRatio, float zNear, float zFar);
		Camera(Camera const &other) = default;
		Camera(Camera &&other) noexcept = default;
		~Camera() noexcept = default;

		Camera &operator=(Camera const &other) = default;
		Camera &operator=(Camera &&other) noexcept = default;

		float GetFoV() const;
		float GetAspectRatio() const;
		glm::vec2 GetClippingPlanes() const;
		glm::vec3 GetUp();
		glm::vec3 GetRight();
		glm::vec3 GetPosition();
		glm::vec3 GetRotation();
		glm::mat4x4 GetView() const;
		glm::mat4x4 GetProjection() const;
		glm::mat4x4 GetViewProjection() const;
		Frustum GetFrustum();

		void SetProjection(float fov, float aspectRatio, float zNear, float zFar);
		void SetView(glm::mat4x4 view);
		void SetViewProjection(glm::mat4x4 view, float fov, float aspectRatio, float zNear, float zFar);
		void UpdateFrustum(glm::vec3 right, glm::vec3 up);
		void UpdateRotationOffset(glm::vec2 rotationOffset);

		/// expected value is postitive value of (camera movement speed * deltaTime)
		void MoveLeft(float distanceToMove);
		/// expected value is postitive value of (camera movement speed * deltaTime)
		void MoveRight(float distanceToMove);
		/// expected value is postitive value of (camera movement speed * deltaTime)
		void MoveForwards(float distanceToMove);
		/// expected value is postitive value of (camera movement speed * deltaTime)
		void MoveBackwards(float distanceToMove);

	private:
		float fov;
		float aspectRatio;
		float horizontalAngle = glm::pi<float>();
		float verticalAngle;
		glm::vec2 clippingPlanes;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::mat4x4 view;
		glm::mat4x4 projection;
		Frustum frustum;
	};
} // namespace Engine
