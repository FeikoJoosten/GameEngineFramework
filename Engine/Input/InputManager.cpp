#include "Engine/Input/InputManager.hpp"
#include "Engine/engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include <windows.h>

class InputCallbacks : public gainput::InputListener
{
public:
	InputCallbacks(gainput::InputManager& manager, int index) : manager(manager), index(index) { }

	bool OnDeviceButtonBool(gainput::DeviceId deviceId, gainput::DeviceButtonId deviceButton, bool oldValue, bool newValue) override
	{
		const gainput::InputDevice* device = manager.GetDevice(deviceId);
		char buttonName[64] = "";
		device->GetButtonName(deviceButton, buttonName, 64);
		ImGuiIO& io = ImGui::GetIO();

		if (deviceId == Engine::Engine::GetEngine().lock()->GetInputManager().lock()->GetKeyboardId())
		{
			io.KeysDown[deviceButton] = newValue;

			if (deviceButton == gainput::KeyCtrlL || deviceButton == gainput::KeyCtrlR)
				io.KeyCtrl = newValue;

			if (deviceButton == gainput::KeyAltL || deviceButton == gainput::KeyAltR)
				io.KeyAlt = newValue;

			if (deviceButton == gainput::KeyShiftL || deviceButton == gainput::KeyShiftR)
				io.KeyShift = newValue;

			if (deviceButton == gainput::KeySuperL || deviceButton == gainput::KeySuperR)
				io.KeySuper = newValue;
		}
		else if (deviceId == Engine::Engine::GetEngine().lock()->GetInputManager().lock()->GetMouseId())
		{
			io.MouseDown[deviceButton] = newValue;

			if (deviceButton == gainput::MouseButton3)
			{
				if (newValue)
					io.MouseWheel += Engine::Engine::GetEngine().lock()->GetInputManager().lock()->GetInputDefaults().scrollSpeed;
			}
			else if (deviceButton == gainput::MouseButton4)
			{
				if (newValue)
					io.MouseWheel -= Engine::Engine::GetEngine().lock()->GetInputManager().lock()->GetInputDefaults().scrollSpeed;
			}

			//printf_s("(%d) Device %d (%s%d) bool button (%d/%s) changed: %d -> %d\n", index, deviceId, device->GetTypeName(), device->GetIndex(), deviceButton, buttonName, oldValue, newValue);
		}

		return false;
	}

	bool OnDeviceButtonFloat(gainput::DeviceId deviceId, gainput::DeviceButtonId deviceButton, float oldValue, float newValue) override
	{
		if (deviceId == Engine::Engine::GetEngine().lock()->GetInputManager().lock()->GetMouseId())
		{
			ImGuiIO& io = ImGui::GetIO();
			if (deviceButton == gainput::MouseAxisX)
			{
				io.MousePos.x = newValue * Engine::Engine::GetEngine().lock()->GetWindow().lock()->GetWidth();
			}
			else if (deviceButton == gainput::MouseAxisY)
			{
				io.MousePos.y = newValue * Engine::Engine::GetEngine().lock()->GetWindow().lock()->GetHeight();
			}
		}

		//printf_s("(%d) Device %d (%s%d) float button (%d/%s) changed: %f -> %f\n", index, deviceId, device->GetTypeName(), device->GetIndex(), deviceButton, buttonName, oldValue, newValue);
		return true;
	}

	int GetPriority() const override
	{
		return index;
	}

private:
	gainput::InputManager& manager;
	int index;
};

namespace Engine
{
	InputManager::InputManager() noexcept : inputManager()
	{
		inputManager.SetDisplaySize(Engine::GetEngine().lock()->GetWindow().lock()->GetWidth(), Engine::GetEngine().lock()->GetWindow().lock()->GetHeight());

		mouseId = inputManager.CreateDevice<gainput::InputDeviceMouse>();
		keyboardId = inputManager.CreateDevice<gainput::InputDeviceKeyboard>();
		gamepadId = inputManager.CreateDevice<gainput::InputDevicePad>();
		inputManager.AddListener(new InputCallbacks(inputManager, 1));

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

	gainput::InputManager& InputManager::GetInputManager()
	{
		return inputManager;
	}

	eastl::vector<gainput::DeviceButtonId> InputManager::GetAllKeysDown(gainput::DeviceId deviceId)
	{
		eastl::vector<gainput::DeviceButtonId> specToReturn(512, false);

		for (int i = 0; i < gainput::KeyCount_; ++i)
		{
			bool value = inputManager.GetDevice(deviceId)->GetBool(i);
			specToReturn[i] = value;
		}

		return specToReturn;
	}

	const gainput::DeviceId& InputManager::GetMouseId() const
	{
		return mouseId;
	}

	const gainput::DeviceId& InputManager::GetKeyboardId() const
	{
		return keyboardId;
	}

	const gainput::DeviceId& InputManager::GetGamepadId() const
	{
		return gamepadId;
	}

	InputDefaults InputManager::GetInputDefaults() const
	{
		return inputDefaults;
	}

	void InputManager::Update() noexcept
	{
		inputManager.Update();

		MSG message;
		while (PeekMessage(&message, Engine::GetEngine().lock()->GetWindow().lock()->GetWindowHandle(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			inputManager.HandleMessage(message);

			if (message.message == WM_CHAR)
			{
				const WPARAM key = message.wParam;
				if (key == 0x08 // backspace 
					|| key == 0x0A // linefeed 
					|| key == 0x1B // escape 
					|| key == 0x09 // tab 
					|| key == 0x0D // carriage return 
					|| key > 255)
				{
					return;
				}
				ImGuiIO& io = ImGui::GetIO();
				io.AddInputCharacter(ImWchar(char(key)));
			}
		}
	}
} // namespace Engine