#pragma once

#include "Engine/api.hpp"
#include "Engine/Camera/Frustum.hpp"
#include <ThirdParty/glm/glm/glm.hpp>

namespace Engine
{
	/// <summary>
	/// This class is used for the creation of a camera.
	/// </summary>
	class ENGINE_API Camera
	{
	public:

		Camera() = delete;
		/// <summary>
		/// Default camera initialization.
		/// </summary>
		/// <param name="position">Use this value to define the starting position of the camera.</param>
		/// <param name="rotation">Use this value to define the starting rotation of the camera.</param>
		/// <param name="fov">Use this value to define the field of view of the camera.</param>
		/// <param name="aspectRatio">Use this value to define the aspect ratio of the camera.</param>
		/// <param name="zNear">Use this value to define the minimal render distance from the camera to where it should start to render.</param>
		/// <param name="zFar">Use this value to define the maximal render distance from the zNear value to where it should end rendering.</param>
		Camera(glm::vec3 position, glm::vec3 rotation, float fov, float aspectRatio, float zNear, float zFar);
		Camera(Camera const &other) = default;
		Camera(Camera &&other) noexcept = default;
		~Camera() noexcept = default;

		Camera &operator=(Camera const &other) = default;
		Camera &operator=(Camera &&other) noexcept = default;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the field of view of the camera.</returns>
		float GetFoV() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the aspect ratio of the camera.</returns>
		float GetAspectRatio() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the zNear and zFar values of the camera. The x of the vector 2 being the zNear and the y of the vector 2 being the zFar.</returns>
		glm::vec2 GetClippingPlanes() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Gets the up direction of the camera.</returns>
		glm::vec3 GetUp() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Gets the right direction of the camera.</returns>
		glm::vec3 GetRight() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Gets the current position of the camera.</returns>
		glm::vec3 GetPosition() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Gets the current rotation of the camera.</returns>
		glm::vec3 GetRotation() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the view matrix of the camera.</returns>
		glm::mat4x4 GetView() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the projection matrix of the camera.</returns>
		glm::mat4x4 GetProjection() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the view * projection matrices.</returns>
		glm::mat4x4 GetViewProjection() const;
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the frustrum class of the camera. Which can be used for frustum culling.</returns>
		Frustum GetFrustum() const;

		/// <summary>
		/// This function allows you to set the projection matrix of the camera. Automatically creates a projection matrix for you.
		/// </summary>
		/// <param name="fov">Use this value to define the field of view of the camera.</param>
		/// <param name="aspectRatio">Use this value to define the aspect ratio of the camera.</param>
		/// <param name="zNear">Use this value to define the minimal render distance from the camera to where it should start to render.</param>
		/// <param name="zFar">Use this value to define the maximal render distance from the zNear value to where it should end rendering.</param>
		void SetProjection(float fov, float aspectRatio, float zNear, float zFar);

		/// <summary>
		/// Use this method to set the view matrix yourself.
		/// </summary>
		/// <param name="view">The new view matrix for the camera.</param>
		void SetView(glm::mat4x4 view);

		/// <summary>
		/// This method allows you to set the view and projection matrices of the camera. This method will simply call the SetView and SetProjection methods for you.
		/// </summary>
		/// <param name="view">The new view matrix for the camera.</param>
		/// <param name="fov">Use this value to define the field of view of the camera.</param>
		/// <param name="aspectRatio">Use this value to define the aspect ratio of the camera.</param>
		/// <param name="zNear">Use this value to define the minimal render distance from the camera to where it should start to render.</param>
		/// <param name="zFar">Use this value to define the maximal render distance from the zNear value to where it should end rendering.</param>
		void SetViewProjection(glm::mat4x4 view, float fov, float aspectRatio, float zNear, float zFar);

		/// <summary>
		/// This method will create a new frustum for you based on the new right and up values.
		/// </summary>
		/// <param name="right">The right direction of the new frustrum</param>
		/// <param name="up">The up direction of the new frustrum</param>
		void UpdateFrustum(glm::vec3 right, glm::vec3 up);

		/// <summary>
		/// This method can be used to update the rotation of the camera based on the defined offset.
		/// </summary>
		/// <param name="rotationOffset"></param>
		void UpdateRotationOffset(glm::vec2 rotationOffset);

		/// <summary>
		/// This method allows you to move X distance to the left axis of the camera.
		/// </summary>
		/// <param name="distanceToMove">expected value is postitive value of (camera movement speed * deltaTime)</param>
		void MoveLeft(float distanceToMove);

		/// <summary>
		/// This method allows you to move X distance to the right axis of the camera.
		/// </summary>
		/// <param name="distanceToMove">expected value is postitive value of (camera movement speed * deltaTime)</param>
		void MoveRight(float distanceToMove);

		/// <summary>
		/// This method allows you to move X distance to the forward axis of the camera.
		/// </summary>
		/// <param name="distanceToMove">expected value is postitive value of (camera movement speed * deltaTime)</param>
		void MoveForwards(float distanceToMove);

		/// <summary>
		/// This method allows you to move X distance to the backward axis of the camera.
		/// </summary>
		/// <param name="distanceToMove">expected value is postitive value of (camera movement speed * deltaTime)</param>
		void MoveBackwards(float distanceToMove);

		/// <summary>
		/// Use this method to set the position and the rotation of the camera.
		/// </summary>
		/// <param name="position">This value is used to define the new position of the camera.</param>
		/// <param name="rotation">This value isu sed to define the new rotation of the camera.</param>
		void SetPostionAndRotation(glm::vec3 position, glm::vec3 rotation);

		/// <summary>
		/// This method allows you to set the position of the camera.
		/// </summary>
		/// <param name="position">The new postion you want to use.</param>
		void SetPosition(glm::vec3 position);

		/// <summary>
		/// This method allows you to set the rotation of the camera.
		/// </summary>
		/// <param name="rotation">The new rotation you want to use.</param>
		void SetRotation(glm::vec3 rotation);

	private:
		float fov;
		float aspectRatio;
		float horizontalAngle;
		float verticalAngle;
		glm::vec2 clippingPlanes;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::mat4x4 view;
		glm::mat4x4 projection;
		Frustum frustum;
	};
} // namespace Engine
