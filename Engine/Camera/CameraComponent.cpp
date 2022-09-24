#include "Engine/Camera/CameraComponent.hpp"
#include "Engine/Window/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Engine {
	CameraComponent::CameraComponent(const float fieldOfVision, const float zNear, const float zFar) : fieldOfVision(fieldOfVision), clippingPlanes(zNear, zFar) {
		SetProjection(fieldOfVision, zNear, zFar);
	}

	CameraComponent::~CameraComponent() {
		if(!transformComponent.expired()) {
			transformComponent.lock()->OnModifiedEvent -= Sharp::EventHandler::Bind(this, &CameraComponent::HandleOnTransformComponentModifiedEvent);
			transformComponent.reset();
		}
	}

	float CameraComponent::GetFieldOfVision() const {
		return fieldOfVision;
	}

	glm::vec2 CameraComponent::GetClippingPlanes() const {
		return clippingPlanes;
	}

	glm::mat4x4 CameraComponent::GetView() const {
		return view;
	}

	glm::mat4x4 CameraComponent::GetProjection() const {
		return projection;
	}

	glm::mat4x4 CameraComponent::GetViewProjection() const {
		return GetView() * GetProjection();
	}

	void CameraComponent::SetProjection(const float newFoV, const float zNear, const float zFar) {
		fieldOfVision = newFoV;
		clippingPlanes.x = zNear;
		clippingPlanes.y = zFar;

		const std::shared_ptr<Window> window = Window::Get();

		projection = glm::perspective(glm::radians(newFoV), static_cast<float>(window->GetHeight()) / static_cast<float>(window->GetWidth()), zNear, zFar);
	}

	void CameraComponent::SetView(const glm::mat4x4 newView) {
		view = newView;
	}

	void CameraComponent::SetViewProjection(const glm::mat4x4 newView, const float newFoV, const float zNear, const float zFar) {
		SetView(newView);
		SetProjection(newFoV, zNear, zFar);
	}

	Frustum CameraComponent::CalculateFrustum(const glm::vec3 right, const glm::vec3 up) const {
		if (!GetIsEnabled() || !transformComponent.expired()) return {};

		const std::shared_ptr<TransformComponent> transformComponentShared = transformComponent.lock();
		const std::shared_ptr<Window> window = Window::Get();
		return Frustum(fieldOfVision, static_cast<float>(window->GetHeight()) / static_cast<float>(window->GetWidth()), transformComponentShared->GetPosition(), transformComponentShared->GetRotation(), clippingPlanes, right, up);
	}

	void CameraComponent::HandleOnTransformComponentModifiedEvent(const std::shared_ptr<TransformComponent> modifiedTransformComponent) {
		if (!GetIsEnabled() || !modifiedTransformComponent) return;
		
		const glm::vec3 position = modifiedTransformComponent->GetPosition();
		SetView(glm::lookAt(position, position + modifiedTransformComponent->GetForward(), modifiedTransformComponent->GetUp()));
	}

	void CameraComponent::OnComponentAdded(const std::shared_ptr<Component> addedComponent) {
		Component::OnComponentAdded(addedComponent);

		if (!transformComponent.expired()) return;

		if (const std::shared_ptr<TransformComponent> possibleTransformComponent = std::dynamic_pointer_cast<TransformComponent>(addedComponent)) {
			transformComponent = possibleTransformComponent;
			possibleTransformComponent->OnModifiedEvent += Sharp::EventHandler::Bind(this, &CameraComponent::HandleOnTransformComponentModifiedEvent);
			SetIsEnabled(true);
			HandleOnTransformComponentModifiedEvent(possibleTransformComponent);
		}
	}

	void CameraComponent::OnComponentRemoved(const std::shared_ptr<Component> removedComponent) {
		Component::OnComponentRemoved(removedComponent);

		if(!transformComponent.expired()) {
			transformComponent.lock()->OnModifiedEvent -= Sharp::EventHandler::Bind(this, &CameraComponent::HandleOnTransformComponentModifiedEvent);
			transformComponent.reset();
			SetIsEnabled(false);
		}
	}
} // namespace Engine
