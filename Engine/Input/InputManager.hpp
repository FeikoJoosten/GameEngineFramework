#pragma once

#include "Engine/api.hpp"
#include "Engine/Window/Window.hpp"

#include <ThirdParty/GainInput/lib/include/gainput/gainput.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	struct InputDefaults
	{
		float scrollSpeed = 5;
	};

	/// <summary>
	/// This object is a simple wrapper class regarding gainput. It also sends information to ImGUI to keep track of pressed buttons. NOTE: only the Engine class is allowed to create this object.
	/// </summary>
	class ENGINE_API InputManager
	{
	public:
		/// <summary>
		/// This method allows you to get the input manager from gainput.
		/// </summary>
		/// <returns>Returns the input manager from gainput as a reference.</returns>
		gainput::InputManager& GetInputManager();

		/// <summary>
		/// This method allows you to get all current keys down.
		/// NOTE: This only works for keboards.
		/// </summary>
		/// <param name="deviceId">The device you want to get the keys for.</param>
		/// <returns>Returns a vector of all current pressed keyboard buttons.</returns>
		eastl::vector<gainput::DeviceButtonId> GetAllKeysDown(gainput::DeviceId deviceId);

		/// <summary>
		/// This method will return the device Id of the linked mouse.
		/// </summary>
		/// <returns>Returns the device Id of the linked mouse as a const reference</returns>
		const gainput::DeviceId& GetMouseId() const;

		/// <summary>
		/// This method will return the device Id of the linked keyboard.
		/// </summary>
		/// <returns>Returns the device Id of the linked keyboard as a const reference</returns>
		const gainput::DeviceId& GetKeyboardId() const;

		/// <summary>
		/// This method will return the device Id of the linked gamepad.
		/// </summary>
		/// <returns>Returns the device Id of the linked gamepad as a const reference</returns>
		const gainput::DeviceId& GetGamepadId() const;

		/// <summary>
		/// This method allows you to retrieve some predefined input defaults.
		/// </summary>
		/// <returns></returns>
		InputDefaults GetInputDefaults() const;
	private:
		gainput::InputManager inputManager;
		gainput::DeviceId mouseId;
		gainput::DeviceId keyboardId;
		gainput::DeviceId gamepadId;
		InputDefaults inputDefaults;

		friend class Engine;
		void Update() noexcept;

		friend void Window::OnWindowResized(GLFWwindow* window, int width, int height);

		InputManager() noexcept;
		InputManager(InputManager const &other) = delete;
		InputManager(InputManager &&other) noexcept = delete;
	public:
		~InputManager() noexcept = default;
	private:

		InputManager &operator=(InputManager const &other) = delete;
		InputManager &operator=(InputManager &&other) noexcept = delete;
	};
} //namespace Engine