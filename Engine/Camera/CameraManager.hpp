#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <memory>
#include <vector>

namespace Engine {
	class CameraComponent;
	class Component;
	class Entity;
	class EntitySystem;

	class ENGINE_API CameraManager {
		friend std::shared_ptr<CameraManager> Engine::GetCameraManager() noexcept;
		friend class Application;

		explicit CameraManager();
	public:
		CameraManager(const CameraManager& other) = delete;
		CameraManager(CameraManager&& other) noexcept = delete;
		~CameraManager();

		CameraManager& operator=(const CameraManager& other) = delete;
		CameraManager& operator=(CameraManager&& other) noexcept = delete;

		static std::shared_ptr<CameraManager> Get();

		[[nodiscard]] const std::vector<std::shared_ptr<CameraComponent>>& GetAllActiveCameras() const;

	private:
		std::shared_ptr<EntitySystem> entitySystem;
		std::vector<std::shared_ptr<CameraComponent>> allCameras;
		std::vector<std::shared_ptr<CameraComponent>> allActiveCameras;

		void HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent);
		void HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent);
		void HandleOnCameraActiveStateChangedEvent(std::shared_ptr<Component> component, bool isEnabled);
	};
}

