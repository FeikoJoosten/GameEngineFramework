#include "Engine/Camera/CameraManager.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Components/CameraComponent.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Entity/EntitySystem.hpp"

namespace Engine {

	CameraManager::CameraManager() : entitySystem(Engine::GetEntitySystem()) {
		if (entitySystem) {
			entitySystem->OnComponentAddedToEntityEvent += Sharp::EventHandler::Bind(this, &CameraManager::HandleOnComponentAddedToEntityEvent);
			entitySystem->OnComponentRemovedFromEntityEvent += Sharp::EventHandler::Bind(this, &CameraManager::HandleOnComponentRemovedFromEntityEvent);
		}
	}

	CameraManager::~CameraManager() {
		if (entitySystem) {
			entitySystem->OnComponentAddedToEntityEvent -= Sharp::EventHandler::Bind(this, &CameraManager::HandleOnComponentAddedToEntityEvent);
			entitySystem->OnComponentRemovedFromEntityEvent -= Sharp::EventHandler::Bind(this, &CameraManager::HandleOnComponentRemovedFromEntityEvent);
		}
	}

	std::shared_ptr<CameraManager> CameraManager::Get() {
		return Engine::GetCameraManager();
	}

	const std::vector<std::shared_ptr<CameraComponent>>& CameraManager::GetAllActiveCameras() const {
		return allActiveCameras;
	}

	void CameraManager::HandleOnComponentAddedToEntityEvent(const std::shared_ptr<Entity> entity, const std::shared_ptr<Component> addedComponent) {
		const std::shared_ptr<CameraComponent> cameraComponent = std::dynamic_pointer_cast<CameraComponent>(addedComponent);
		if (!cameraComponent) return;

		allCameras.push_back(cameraComponent);
		if (cameraComponent->GetIsActiveAndEnabled()) allActiveCameras.push_back(cameraComponent);

		cameraComponent->OnEnabledStateChangedEvent += Sharp::EventHandler::Bind(this, &CameraManager::HandleOnCameraActiveStateChangedEvent);
	}

	void CameraManager::HandleOnComponentRemovedFromEntityEvent(const std::shared_ptr<Entity> entity, const std::shared_ptr<Component> removedComponent) {
		const std::shared_ptr<CameraComponent> cameraComponent = std::dynamic_pointer_cast<CameraComponent>(removedComponent);
		if (!cameraComponent) return;

		cameraComponent->OnEnabledStateChangedEvent -= Sharp::EventHandler::Bind(this, &CameraManager::HandleOnCameraActiveStateChangedEvent);

		allCameras.erase(
			std::remove_if(allCameras.begin(), allCameras.end(),
				[cameraComponent](const std::shared_ptr<CameraComponent>& component) {
					return component.get() == cameraComponent.get();
				}), allCameras.end());

		allActiveCameras.erase(
			std::remove_if(allActiveCameras.begin(), allActiveCameras.end(),
				[cameraComponent](const std::shared_ptr<CameraComponent>& component) {
					return component.get() == cameraComponent.get();
				}), allActiveCameras.end());
	}

	void CameraManager::HandleOnCameraActiveStateChangedEvent(const std::shared_ptr<Component> component, const bool isEnabled) {
		const std::shared_ptr<CameraComponent> cameraComponent = std::dynamic_pointer_cast<CameraComponent>(component);
		if (!cameraComponent) return; // Should never happen, but just to be safe

		if (isEnabled)
			allActiveCameras.push_back(cameraComponent);
		else {
			allActiveCameras.erase(
				std::remove_if(allActiveCameras.begin(), allActiveCameras.end(),
					[cameraComponent](const std::shared_ptr<CameraComponent>& component) {
						return component.get() == cameraComponent.get();
					}), allActiveCameras.end());
		}
	}
}
