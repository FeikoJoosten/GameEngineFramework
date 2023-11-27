#include "Engine/Components/CameraComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Camera/Frustum.hpp"
#include "Engine/Window/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

CEREAL_REGISTER_TYPE(Engine::CameraComponent)
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(Engine::CameraComponent, specialization::member_load_save)

namespace Engine {
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

	glm::mat4x4 CameraComponent::GetProjectionView() const {
		return GetProjection() * GetView();
	}

	void CameraComponent::SetProjection(const float newFoV, const float zNear, const float zFar) {
		fieldOfVision = newFoV;
		clippingPlanes.x = zNear;
		clippingPlanes.y = zFar;

		const std::shared_ptr<Window>& window = Window::Get();

		const float aspectRatio = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
		projection = glm::perspective(glm::radians(newFoV), aspectRatio, zNear, zFar);
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
		const glm::vec3 center = position + modifiedTransformComponent->GetForward();
		SetView(glm::lookAt(position, center, modifiedTransformComponent->GetUp()));
	}

	void CameraComponent::OnComponentAdded(const std::shared_ptr<Component> addedComponent) {
		Component::OnComponentAdded(addedComponent);

		if (!transformComponent.expired()) return;

		if (const std::shared_ptr<TransformComponent> possibleTransformComponent = std::dynamic_pointer_cast<TransformComponent>(addedComponent)) {
			transformComponent = possibleTransformComponent;
			possibleTransformComponent->OnModifiedEvent += Sharp::EventHandler::Bind(this, &CameraComponent::HandleOnTransformComponentModifiedEvent);
			HandleOnTransformComponentModifiedEvent(possibleTransformComponent);
		}
	}

	void CameraComponent::OnComponentRemoved(const std::shared_ptr<Component> removedComponent) {
		Component::OnComponentRemoved(removedComponent);

		if(!transformComponent.expired()) {
			transformComponent.lock()->OnModifiedEvent -= Sharp::EventHandler::Bind(this, &CameraComponent::HandleOnTransformComponentModifiedEvent);
			transformComponent.reset();
		}
	}
}
