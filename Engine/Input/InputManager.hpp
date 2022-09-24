#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Window/Window.hpp"

#include <gainput/gainput.h>

class InputCallbacks;

namespace Engine {
	struct ENGINE_API InputDefaults {
		float scrollSpeed = 5;
	};

	/// <summary>
	/// This object is a simple wrapper class regarding Gainput. It also sends information to ImGUI to keep track of pressed buttons.
	/// </summary>
	class ENGINE_API InputManager {
		friend std::shared_ptr<InputManager> Engine::GetInputManager() noexcept;
		friend void Application::Update();

		InputManager() noexcept;
	public:
		~InputManager() noexcept;
		InputManager(const InputManager& other) = delete;
		InputManager(InputManager&& other) noexcept = delete;

		InputManager& operator=(const InputManager& other) = delete;
		InputManager& operator=(InputManager&& other) noexcept = delete;

		static std::shared_ptr<InputManager> Get();

		/// <summary> This method allows you to create a new input map from Gainput.</summary>
		/// <returns>Returns the newly created input map.</returns>
		[[nodiscard]] std::shared_ptr<gainput::InputMap> CreateInputMap();

		/// <summary> This method allows you to create a new input device from Gainput.</summary>
		///	<param name="deviceType">The type of device to create</param>
		///	<param name="playerIndex">The player index that should be used in case of multiple devices being connected</param>
		/// <returns>Returns the DeviceId of the newly created input device.</returns>
		[[nodiscard]] gainput::DeviceId GetOrCreateDeviceForType(gainput::InputDevice::DeviceType deviceType, int playerIndex = 0);

		/// <summary>
		/// This method allows you to retrieve some predefined input defaults.
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] InputDefaults GetInputDefaults() const;
	private:
		gainput::InputManager inputManager {};
		InputDefaults inputDefaults {};
		std::unique_ptr<InputCallbacks> inputCallbacks {};
		gainput::ListenerId inputCallbacksListenerId;
		std::shared_ptr<Window> window {};

		void Update() noexcept;

		void HandleOnWindowResizedEvent(GLFWwindow* glfwWindow, int newWidth, int newHeight);
	};
} //namespace Engine