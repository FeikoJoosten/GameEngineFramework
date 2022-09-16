#include "Engine/Camera/CameraManager.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Camera/CameraComponent.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Entity/EntitySystem.hpp"

namespace Engine {

	CameraManager::CameraManager() : entitySystem(Engine::GetEntitySystem()){
		if (entitySystem) {
			entitySystem->OnComponentAddedToEntityEvent += Sharp::EventHandler::Bind(this, &CameraManager::HandleOnComponentAddedToEntity);
			entitySystem->OnComponentRemovedFromEntityEvent += Sharp::EventHandler::Bind(this, &CameraManager::HandleOnComponentRemovedFromEntity);
		}
	}

	CameraManager::~CameraManager() {
		if (entitySystem) {
			entitySystem->OnComponentAddedToEntityEvent -= Sharp::EventHandler::Bind(this, &CameraManager::HandleOnComponentAddedToEntity);
			entitySystem->OnComponentRemovedFromEntityEvent -= Sharp::EventHandler::Bind(this, &CameraManager::HandleOnComponentRemovedFromEntity);
		}
	}

	std::shared_ptr<CameraManager> CameraManager::Get() {
		return Engine::GetCameraManager();
	}

	std::vector<std::shared_ptr<CameraComponent>> CameraManager::GetAllActiveCameras() const {
		std::vector<std::shared_ptr<CameraComponent>> activeCameras = {};

		for(const std::shared_ptr<CameraComponent>& cameraComponent : allCameras) {
			if (!cameraComponent->GetIsEnabled() || !cameraComponent->GetOwner().lock()->GetIsActive()) continue;

			activeCameras.push_back(cameraComponent);
		}

		return activeCameras;
	}

	void CameraManager::HandleOnComponentAddedToEntity(const std::shared_ptr<Entity> entity, const std::shared_ptr<Component> addedComponent) {
		const std::shared_ptr<CameraComponent> cameraComponent = std::dynamic_pointer_cast<CameraComponent>(addedComponent);
		if (!cameraComponent) return;

		// TODO: Make use of component OnActiveChangedEvent once available
		allCameras.push_back(cameraComponent);
	}

	void CameraManager::HandleOnComponentRemovedFromEntity(const std::shared_ptr<Entity> entity, const std::shared_ptr<Component> removedComponent) {
		const std::shared_ptr<CameraComponent> cameraComponent = std::dynamic_pointer_cast<CameraComponent>(removedComponent);
		if (!cameraComponent) return;
		
		allCameras.erase(
		std::remove_if(allCameras.begin(), allCameras.end(),
			[cameraComponent](const std::shared_ptr<CameraComponent>& component) {
				return component.get() == cameraComponent.get();
			}), allCameras.end());
	}
}
