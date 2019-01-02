#pragma once

#include "Engine/api.hpp"
#include <ThirdParty/GainInput/lib/include/gainput/gainput.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

#if defined(GAINPUT_PLATFORM_WIN)

namespace Engine
{
	struct InputDefaults
	{
		float scrollSpeed = 5;
	};

	class ENGINE_API InputManager
	{
	public:

		InputManager() noexcept;
		InputManager(InputManager const &other) = default;
		InputManager(InputManager &&other) noexcept = default;
		~InputManager() noexcept = default;

		InputManager &operator=(InputManager const &other) = default;
		InputManager &operator=(InputManager &&other) noexcept = default;

		void MapBool(gainput::UserButtonId button, gainput::DeviceId deviceID, gainput::DeviceButtonId buttonID);
		bool GetBool(gainput::UserButtonId button);
		bool GetBoolWasDown(gainput::UserButtonId button);
		void MapFloat(gainput::UserButtonId button, gainput::DeviceId deviceID, gainput::DeviceButtonId buttonID);
		float GetFloat(gainput::UserButtonId button);
		float GetFloatDelta(gainput::UserButtonId button);
		const gainput::InputDevice* GetDevice(gainput::DeviceId deviceID);
		eastl::vector<gainput::DeviceButtonId> GetAllKeysDown(gainput::DeviceId deviceID);

		gainput::DeviceId GetMouseID();
		gainput::DeviceId GetKeyboardID();
		void Update() noexcept;

		InputDefaults GetInputDefaults();
	private:
		gainput::InputManager inputManager;
		gainput::DeviceId mouseID;
		gainput::DeviceId keyboardID;
		gainput::InputMap inputMap;
		InputDefaults inputDefaults;
	};
} //namespace Engine

#endif