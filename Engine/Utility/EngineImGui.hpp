#pragma once

namespace Engine
{
	class EngineImGui
	{
	public:
		EngineImGui() = default;
		~EngineImGui() = default;

		bool allowCameraMovement = false;
		void DevMenu();
		void CameraMovement();
	};
} //namespace Engine