#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window/Window.hpp"

#include <vector>
#include <gainput/gainput.h>

namespace Engine {
	struct InputDefaults {
		float scrollSpeed = 5;
	};

	/// <summary>
	/// This object is a simple wrapper class regarding Gainput. It also sends information to ImGUI to keep track of pressed buttons. NOTE: only the Engine class is allowed to create this object.
	/// </summary>
	class ENGINE_API InputManager {
		friend std::shared_ptr<InputManager> Engine::GetInputManager() noexcept;
		friend class Application;

		InputManager() noexcept;
	public:
		~InputManager() noexcept;
		InputManager(const InputManager& other) = delete;
		InputManager(InputManager&& other) noexcept = delete;

		InputManager& operator=(const InputManager& other) = delete;
		InputManager& operator=(InputManager&& other) noexcept = delete;

		static std::shared_ptr<InputManager> Get();

		/// <summary>
		/// This method allows you to get the input manager from Gainput.
		/// </summary>
		/// <returns>Returns the input manager from Gainput as a reference.</returns>
		gainput::InputManager& GetInputManager();

		/// <summary>
		/// This method allows you to get all current keys down.
		/// NOTE: This only works for keyboards.
		/// </summary>
		/// <param name="deviceId">The device you want to get the keys for.</param>
		/// <returns>Returns a vector of all current pressed keyboard buttons.</returns>
		std::vector<gainput::DeviceButtonId> GetAllKeysDown(gainput::DeviceId deviceId);

		/// <summary>
		/// This method will return the device Id of the linked mouse.
		/// </summary>
		/// <returns>Returns the device Id of the linked mouse as a const reference</returns>
		[[nodiscard]] const gainput::DeviceId& GetMouseId() const;

		/// <summary>
		/// This method will return the device Id of the linked keyboard.
		/// </summary>
		/// <returns>Returns the device Id of the linked keyboard as a const reference</returns>
		[[nodiscard]] const gainput::DeviceId& GetKeyboardId() const;

		/// <summary>
		/// This method will return the device Id of the linked gamepad.
		/// </summary>
		/// <returns>Returns the device Id of the linked gamepad as a const reference</returns>
		[[nodiscard]] const gainput::DeviceId& GetGamepadId() const;

		/// <summary>
		/// This method allows you to retrieve some predefined input defaults.
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] InputDefaults GetInputDefaults() const;
	private:
		gainput::InputManager inputManager;
		gainput::DeviceId mouseId;
		gainput::DeviceId keyboardId;
		gainput::DeviceId gamepadId;
		InputDefaults inputDefaults;
		std::shared_ptr<Window> window;

		void Update() noexcept;

		void HandleOnWindowResizedEvent(GLFWwindow* glfwWindow, int newWidth, int newHeight);
	};
} //namespace Engine