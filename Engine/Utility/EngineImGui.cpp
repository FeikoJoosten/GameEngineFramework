#include "Engine/Utility/EngineImGui.hpp"
#include "Engine/engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"

#include <ThirdParty/glm/glm/gtx/string_cast.hpp>
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>

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

	ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_AutoSelectAll;
	float devMenuCooldown = 0.f;
	bool showMenuBar = false;

	EngineImGui::EngineImGui()
	{
		Engine::GetEngine().lock()->GetRenderer().lock()->OnRender += Sharp::EventHandler::Bind(&EngineImGui::Render, this);
	}

	EngineImGui::~EngineImGui()
	{
		Engine::GetEngine().lock()->GetRenderer().lock()->OnRender -= Sharp::EventHandler::Bind(&EngineImGui::Render, this);
	}

	void EngineImGui::Render()
	{
		if (allowCameraMovement)
			CameraMovement();

		const gainput::InputDevice* keyboard = Engine::GetEngine().lock()->GetInputManager().lock()->GetInputManager().GetDevice(
			Engine::GetEngine().lock()->GetInputManager().lock()->GetKeyboardId());

		if (keyboard->GetBool(gainput::KeyCtrlL) && keyboard->GetBool(gainput::KeyShiftL) && keyboard->GetBool(gainput::KeyE))
		{
			if (devMenuCooldown < 0.f)
			{
				showMenuBar = !showMenuBar;
				devMenuCooldown = 0.25f;
			}
		}
		if (devMenuCooldown >= 0.f)
			devMenuCooldown -= Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime();

		if (showMenuBar)
		{
			DevMenu();
		}
	}

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
					Engine::Engine::GetEngine().lock()->GetWindow().lock()->SetShouldClose(true);
				}
				if (ImGui::MenuItem("Toggle Camera Movement"))
				{
					allowCameraMovement = !allowCameraMovement;
				}
				if (ImGui::MenuItem("Toggle Play/Pause mode"))
				{
					Engine::GetEngine().lock()->SetIsPlaying(!Engine::GetEngine().lock()->GetIsPlaying());
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

		const eastl::vector<float> deltaTimes = Engine::GetEngine().lock()->GetTime().lock()->GetPreviousFramerates();
		ImGui::PlotLines("FPS", deltaTimes.data(), Engine::GetEngine().lock()->GetTime().lock()->GetMaxIterations(), 0, std::to_string(ImGui::GetIO().Framerate).c_str());

		ImGui::End();
	}

	void EngineImGui::CameraMovement() const
	{
		float camMoveSpeed = 100.f;
		eastl::weak_ptr<Camera> camera = Engine::GetEngine().lock()->GetCamera();

		const gainput::InputDevice* keyboard = Engine::GetEngine().lock()->GetInputManager().lock()->GetInputManager().GetDevice(
			Engine::GetEngine().lock()->GetInputManager().lock()->GetKeyboardId());

		if (keyboard->GetBool(gainput::KeyW) && !ImGui::GetIO().WantTextInput)
		{
			camera.lock()->MoveForwards(Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime() * camMoveSpeed);
		}
		if (keyboard->GetBool(gainput::KeyS) && !ImGui::GetIO().WantTextInput)
		{
			camera.lock()->MoveBackwards(Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime() * camMoveSpeed);
		}
		if (keyboard->GetBool(gainput::KeyA) && !ImGui::GetIO().WantTextInput)
		{
			camera.lock()->MoveLeft(Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime() * camMoveSpeed);
		}
		if (keyboard->GetBool(gainput::KeyD) && !ImGui::GetIO().WantTextInput)
		{
			camera.lock()->MoveRight(Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime() * camMoveSpeed);
		}
		glm::vec3 target = glm::normalize(camera.lock()->GetPosition() + camera.lock()->GetRotation());
		camera.lock()->SetView(glm::lookAt(camera.lock()->GetPosition(), camera.lock()->GetPosition() - target, camera.lock()->GetUp()));
	}
} //namespace Engine