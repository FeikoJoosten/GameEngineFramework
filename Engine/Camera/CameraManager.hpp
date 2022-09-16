#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <memory>
#include <vector>

namespace Engine {
	class CameraComponent;
	class Component;
	class Entity;
	class EntitySystem;

	// TODO: Comments
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

		[[nodiscard]] std::vector<std::shared_ptr<CameraComponent>> GetAllActiveCameras() const;

	private:
		std::shared_ptr<EntitySystem> entitySystem;
		std::vector<std::shared_ptr<CameraComponent>> allCameras;

		void HandleOnComponentAddedToEntity(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent);
		void HandleOnComponentRemovedFromEntity(const std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent);
	};
}

