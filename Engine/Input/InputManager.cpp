#include "Engine/Input/InputManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"

#include <Windows.h>

class InputCallbacks final : public gainput::InputListener {
public:
	explicit InputCallbacks(gainput::InputManager& manager) : manager(manager) {}

	virtual bool OnDeviceButtonBool(const gainput::DeviceId deviceId, const gainput::DeviceButtonId deviceButton, bool oldValue, const bool newValue) override {
		const gainput::InputDevice* device = manager.GetDevice(deviceId);
		ImGuiIO& io = ImGui::GetIO();
		const std::shared_ptr<Engine::InputManager> inputManager = Engine::InputManager::Get();
		const gainput::InputDevice::DeviceType deviceType = device->GetType();

		if (deviceType == gainput::InputDevice::DT_KEYBOARD) {
			io.KeysDown[deviceButton] = newValue;

			if (deviceButton == gainput::KeyCtrlL || deviceButton == gainput::KeyCtrlR)
				io.KeyCtrl = newValue;

			if (deviceButton == gainput::KeyAltL || deviceButton == gainput::KeyAltR)
				io.KeyAlt = newValue;

			if (deviceButton == gainput::KeyShiftL || deviceButton == gainput::KeyShiftR)
				io.KeyShift = newValue;

			if (deviceButton == gainput::KeySuperL || deviceButton == gainput::KeySuperR)
				io.KeySuper = newValue;
		} else if (deviceType == gainput::InputDevice::DT_MOUSE) {
			io.MouseDown[deviceButton] = newValue;

			if (deviceButton == gainput::MouseButton3) {
				if (newValue)
					io.MouseWheel += inputManager->GetInputDefaults().scrollSpeed;
			} else if (deviceButton == gainput::MouseButton4) {
				if (newValue)
					io.MouseWheel -= inputManager->GetInputDefaults().scrollSpeed;
			}
		}

		return false;
	}

	virtual bool OnDeviceButtonFloat(const gainput::DeviceId deviceId, const gainput::DeviceButtonId deviceButton, float oldValue, const float newValue) override {
		const gainput::InputDevice::DeviceType deviceType = manager.GetDevice(deviceId)->GetType();

		if (deviceType == gainput::InputDevice::DT_MOUSE) {
			ImGuiIO& io = ImGui::GetIO();
			const std::shared_ptr<Engine::Window> window = Engine::Window::Get();
			if (deviceButton == gainput::MouseAxisX)
				io.MousePos.x = newValue * static_cast<float>(window->GetWidth());
			else if (deviceButton == gainput::MouseAxisY)
				io.MousePos.y = newValue * static_cast<float>(window->GetHeight());
		}

		return true;
	}

private:
	gainput::InputManager& manager;
};

namespace Engine {
	InputManager::InputManager() noexcept {
		window = Window::Get();
		if (window) {
			inputManager.SetDisplaySize(window->GetWidth(), window->GetHeight());

			window->OnWindowResizedEvent += Sharp::EventHandler::Bind(this, &InputManager::HandleOnWindowResizedEvent);
		}

		inputCallbacks = std::make_unique<InputCallbacks>(inputManager);
		inputCallbacksListenerId = inputManager.AddListener(inputCallbacks.get());

		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab] = gainput::KeyTab;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_LeftArrow] = gainput::KeyLeft;
		io.KeyMap[ImGuiKey_RightArrow] = gainput::KeyRight;
		io.KeyMap[ImGuiKey_UpArrow] = gainput::KeyUp;
		io.KeyMap[ImGuiKey_DownArrow] = gainput::KeyDown;
		io.KeyMap[ImGuiKey_PageUp] = gainput::KeyPageUp;
		io.KeyMap[ImGuiKey_PageDown] = gainput::KeyPageDown;
		io.KeyMap[ImGuiKey_Home] = gainput::KeyHome;
		io.KeyMap[ImGuiKey_End] = gainput::KeyEnd;
		io.KeyMap[ImGuiKey_Delete] = gainput::KeyDelete;
		io.KeyMap[ImGuiKey_Backspace] = gainput::KeyBackSpace;
		io.KeyMap[ImGuiKey_Enter] = gainput::KeyReturn;
		io.KeyMap[ImGuiKey_Escape] = gainput::KeyEscape;
		io.KeyMap[ImGuiKey_A] = gainput::KeyA;
		io.KeyMap[ImGuiKey_C] = gainput::KeyC;
		io.KeyMap[ImGuiKey_V] = gainput::KeyV;
		io.KeyMap[ImGuiKey_X] = gainput::KeyX;
		io.KeyMap[ImGuiKey_Y] = gainput::KeyY;
		io.KeyMap[ImGuiKey_Z] = gainput::KeyZ;
	}

	InputManager::~InputManager() noexcept {
		inputManager.RemoveListener(inputCallbacksListenerId);
		if (window)
			window->OnWindowResizedEvent -= Sharp::EventHandler::Bind(this, &InputManager::HandleOnWindowResizedEvent);
	}

	std::shared_ptr<InputManager> InputManager::Get() {
		return Engine::GetInputManager();
	}

	std::shared_ptr<gainput::InputMap> InputManager::CreateInputMap() {
		return std::make_shared<gainput::InputMap>(inputManager);
	}

	gainput::DeviceId InputManager::GetOrCreateDeviceForType(const gainput::InputDevice::DeviceType deviceType, const int playerIndex) {
		const gainput::DeviceId alreadyCreatedDeviceId = inputManager.FindDeviceId(deviceType, playerIndex);
		if (alreadyCreatedDeviceId != gainput::InvalidDeviceId) return alreadyCreatedDeviceId;

		const gainput::InputDevice* createdInputDevice = nullptr;
		switch (deviceType) {
		case gainput::InputDevice::DT_MOUSE:
			createdInputDevice = inputManager.CreateAndGetDevice<gainput::InputDeviceMouse>(playerIndex);
			break;
		case gainput::InputDevice::DT_KEYBOARD:
			createdInputDevice = inputManager.CreateAndGetDevice<gainput::InputDeviceKeyboard>(playerIndex);
			break;
		case gainput::InputDevice::DT_PAD:
			createdInputDevice = inputManager.CreateAndGetDevice<gainput::InputDevicePad>(playerIndex);
			break;
		case gainput::InputDevice::DT_TOUCH:
			createdInputDevice = inputManager.CreateAndGetDevice<gainput::InputDeviceTouch>(playerIndex);
			break;
		case gainput::InputDevice::DT_BUILTIN:
			createdInputDevice = inputManager.CreateAndGetDevice<gainput::InputDeviceBuiltIn>(playerIndex);
			break;
		case gainput::InputDevice::DT_REMOTE:
		case gainput::InputDevice::DT_GESTURE:
		case gainput::InputDevice::DT_CUSTOM:
		case gainput::InputDevice::DT_COUNT:
			throw std::runtime_error("Unsupported device type!");
		}

		if (!createdInputDevice) throw std::runtime_error("Failed to created InputDevice");

		return createdInputDevice->GetDeviceId();
	}

	InputDefaults InputManager::GetInputDefaults() const {
		return inputDefaults;
	}

	void InputManager::Update() noexcept {
		inputManager.Update();

		MSG message;
		while (PeekMessage(&message, Window::Get()->GetWindowHandle(), 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);

			inputManager.HandleMessage(message);

			if (message.message == WM_CHAR) {
				const WPARAM key = message.wParam;
				if (key == 0x08 // backspace 
					|| key == 0x0A // linefeed 
					|| key == 0x1B // escape 
					|| key == 0x09 // tab 
					|| key == 0x0D // carriage return 
					|| key > 255) {
					return;
				}
				ImGuiIO& io = ImGui::GetIO();
				io.AddInputCharacter(static_cast<ImWchar>(key));
			}
		}
	}

	void InputManager::HandleOnWindowResizedEvent(GLFWwindow* glfwWindow, const int newWidth, const int newHeight) {
		inputManager.SetDisplaySize(newWidth, newHeight);
	}
} // namespace Engine