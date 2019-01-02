#include "Engine/Utility/EngineImGui.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"
#include "Engine/engine.hpp"
#include <ThirdParty/glm/glm/gtx/string_cast.hpp>

namespace Engine
{
	bool open = true;

	ImGuiWindowFlags menuBarWindowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_MenuBar;

	ImGuiWindowFlags fpsWindowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse;

	void EngineImGui::DevMenu()
	{
		ImGui::Begin("Menubar demo", &open, menuBarWindowFlags);

		ImGui::BeginMenuBar();
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::BeginMenu("Engine"))
			{
				if (ImGui::MenuItem("Close game"))
				{
					Engine::Engine::GetWindow().SetShouldClose(true);
				}
				if (ImGui::MenuItem("Toggle Camera Movement"))
				{
					allowCameraMovement = !allowCameraMovement;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
		ImGui::End();

		ImGui::SetNextWindowSize(ImVec2(175, 50));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 175, 0));
		ImGui::Begin("FPS window", &open, fpsWindowFlags);

		const eastl::vector<float> deltaTimes = Engine::Engine::GetTime().GetPreviousFramerates();
		ImGui::PlotLines("FPS", deltaTimes.data(), Engine::Engine::GetTime().GetMaxIterations(), 0, std::to_string(ImGui::GetIO().Framerate).c_str());

		ImGui::End();
	}
	
	void EngineImGui::CameraMovement()
	{
		if (!allowCameraMovement) return;

		float camMoveSpeed = 100.f;
		Camera& camera = Engine::GetCamera();

		const gainput::InputDevice* keyboard = Engine::GetInputManager().GetDevice(Engine::GetInputManager().GetKeyboardID());
		/*const gainput::InputDevice* mouse = Engine::GetInputManager().GetDevice(Engine::GetInputManager().GetMouseID());

		glm::vec2 cameraOffset =	glm::vec2(	mouse->GetFloat(gainput::MouseAxisX), 
									mouse->GetFloat(gainput::MouseAxisY)) - 
						glm::vec2(	mouse->GetFloatPrevious(gainput::MouseAxisX), 
									mouse->GetFloatPrevious(gainput::MouseAxisY));
		camera.UpdateRotationOffset( 0.5f * -cameraOffset);*/

		if (keyboard->GetBool(gainput::KeyW))
		{
			camera.MoveForwards(Engine::GetTime().GetDeltaTime() * camMoveSpeed);
		}
		if (keyboard->GetBool(gainput::KeyS))
		{
			camera.MoveBackwards(Engine::GetTime().GetDeltaTime() * camMoveSpeed);
		}
		if (keyboard->GetBool(gainput::KeyA))
		{
			camera.MoveLeft(Engine::GetTime().GetDeltaTime() * camMoveSpeed);
		}
		if (keyboard->GetBool(gainput::KeyD))
		{
			camera.MoveRight(Engine::GetTime().GetDeltaTime() * camMoveSpeed);
		}
		glm::vec3 target = glm::normalize(camera.GetPosition() + camera.GetRotation());
		std::cout << "Camera pos: " << glm::to_string(camera.GetPosition()) << " Target: " << glm::to_string(camera.GetPosition() - target) << std::endl;
		camera.SetView(glm::lookAt(camera.GetPosition(), camera.GetPosition() - target, camera.GetUp()));
	}
} //namespace Engine