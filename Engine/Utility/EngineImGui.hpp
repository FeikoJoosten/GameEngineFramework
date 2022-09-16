#pragma once

#include <memory>

namespace Engine
{
	class Renderer;
	class InputManager;
	class CameraComponent;

	class EngineImGui
	{
		friend class std::shared_ptr<EngineImGui>;

	public:
		EngineImGui();
		~EngineImGui();
		EngineImGui(const EngineImGui& other) = delete;
		EngineImGui(EngineImGui&& other) noexcept = delete;

		EngineImGui& operator=(const EngineImGui& other) = delete;
		EngineImGui& operator=(EngineImGui&& other) noexcept = delete;

	private:
		bool allowCameraMovement = false;
		std::weak_ptr<CameraComponent> activeCamera;
		std::weak_ptr<InputManager> inputManager;
		std::weak_ptr<Renderer> renderer;

		void Render();

		void DevMenu();

		void CameraMovement();
	};
} //namespace Engine