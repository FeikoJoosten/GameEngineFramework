#pragma once

#include "Engine/Api.hpp"
#include "Engine/Camera/Frustum.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Entity/Entity.hpp"

namespace Engine {
	/// <summary>
	/// This class is used for the creation of a camera.
	/// </summary>
	class ENGINE_API CameraComponent : public Component {
		template <class ComponentType, class... Args>
		friend std::shared_ptr<ComponentType> Entity::AddComponent(Args&&... args);

		/// <summary>
		/// Default camera initialization.
		/// </summary>
		/// <param name="fieldOfVision">Use this value to define the field of view of the camera.</param>
		/// <param name="zNear">Use this value to define the minimal render distance from the camera to where it should start to render.</param>
		/// <param name="zFar">Use this value to define the maximal render distance from the zNear value to where it should end rendering.</param>
		CameraComponent(float fieldOfVision, float zNear, float zFar);

	public:
		CameraComponent() = delete;
		CameraComponent(const CameraComponent& other) = delete;
		CameraComponent(CameraComponent&& other) noexcept = delete;
		virtual ~CameraComponent() override;

		CameraComponent& operator=(const CameraComponent& other) = delete;
		CameraComponent& operator=(CameraComponent&& other) noexcept = delete;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the field of view of the camera.</returns>
		[[nodiscard]] float GetFieldOfVision() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>
		/// Returns the zNear and zFar values of the camera.
		/// The x of the vector 2 being the zNear and the y of the vector 2 being the zFar.
		/// </returns>
		[[nodiscard]] glm::vec2 GetClippingPlanes() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the view matrix of the camera.</returns>
		[[nodiscard]] glm::mat4x4 GetView() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the projection matrix of the camera.</returns>
		[[nodiscard]] glm::mat4x4 GetProjection() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the view * projection matrices.</returns>
		[[nodiscard]] glm::mat4x4 GetViewProjection() const;

		/// <summary>
		/// This function allows you to set the projection matrix of the camera. Automatically creates a projection matrix for you.
		/// </summary>
		/// <param name="newFoV">Use this value to define the field of view of the camera.</param>
		/// <param name="zNear">Use this value to define the minimal render distance from the camera to where it should start to render.</param>
		/// <param name="zFar">Use this value to define the maximal render distance from the zNear value to where it should end rendering.</param>
		void SetProjection(float newFoV, float zNear, float zFar);

		/// <summary>
		/// Use this method to set the view matrix yourself.
		/// </summary>
		/// <param name="newView">The new view matrix for the camera.</param>
		void SetView(glm::mat4x4 newView);

		/// <summary>
		/// This method allows you to set the view and projection matrices of the camera. This method will simply call the SetView and SetProjection methods for you.
		/// </summary>
		/// <param name="newView">The new view matrix for the camera.</param>
		/// <param name="newFoV">Use this value to define the field of view of the camera.</param>
		/// <param name="zNear">Use this value to define the minimal render distance from the camera to where it should start to render.</param>
		/// <param name="zFar">Use this value to define the maximal render distance from the zNear value to where it should end rendering.</param>
		void SetViewProjection(glm::mat4x4 newView, float newFoV, float zNear, float zFar);

		/// <summary>
		/// This method will create a new frustum for you based on the new right and up values.
		/// </summary>
		/// <param name="right">The right direction of the new frustum</param>
		/// <param name="up">The up direction of the new frustum</param>
		[[nodiscard]] Frustum CalculateFrustum(glm::vec3 right, glm::vec3 up) const;

	private:
		float fieldOfVision;
		glm::vec2 clippingPlanes;
		glm::mat4x4 view{};
		glm::mat4x4 projection{};
		std::weak_ptr<TransformComponent> transformComponent;

		void HandleOnTransformComponentModifiedEvent(std::shared_ptr<TransformComponent> modifiedTransformComponent);

		virtual void OnComponentAdded(std::shared_ptr<Component> addedComponent) override;

		virtual void OnComponentRemoved(std::shared_ptr<Component> removedComponent) override;
	};
} // namespace Engine
