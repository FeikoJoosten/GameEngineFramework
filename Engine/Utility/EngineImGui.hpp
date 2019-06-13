#pragma once
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>

namespace Engine
{
	class EngineImGui
	{
	private:
		friend class Engine;
		friend class eastl::shared_ptr<EngineImGui>;

		EngineImGui();
	public:
		~EngineImGui();
	private:

		void Render();

		bool allowCameraMovement = false;
		/// <summary>
		/// This method will render the developer menu.
		/// </summary>
		void DevMenu();
		/// <summary>
		/// This method will handle the camera movement.
		/// </summary>
		void CameraMovement() const;
	};
} //namespace Engine