#include "Engine/Utility/EngineImGui.hpp"

#include "Engine/Application/Application.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Camera/CameraManager.hpp"
#include "Engine/Camera/CameraComponent.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Utility/Time.hpp"

#include <gainput/gainput.h>
#include <gainput/GainputInputMap.h>

namespace Engine {
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
	bool showMenuBar = true;

	enum CameraButtons {
		MoveLeft,
		MoveRight,
		MoveForwards,
		MoveBackwards,
		MoveUp,
		MoveDown,
		RotateLeft,
		RotateRight,
		RotateUp,
		RotateDown,
		EnableMouseRotation,
		MouseX,
		MouseY
	};

	gainput::InputMap* map = nullptr;

	EngineImGui::EngineImGui() {
		inputManager = InputManager::Get();
		renderer = Renderer::Get();
		renderer.lock()->PostRenderComponentsRenderEvent += Sharp::EventHandler::Bind(this, &EngineImGui::Render);
		previousFramerates.resize(maxIterations);

		const std::shared_ptr<InputManager> lockedInputManager = inputManager.lock();
		const gainput::DeviceId keyboardDeviceId = lockedInputManager->GetKeyboardId();
		const gainput::DeviceId mouseDeviceId = lockedInputManager->GetMouseId();

		map = new gainput::InputMap(lockedInputManager->GetInputManager());
		map->MapBool(MoveLeft, keyboardDeviceId, gainput::KeyA);
		map->MapBool(MoveRight, keyboardDeviceId, gainput::KeyD);
		map->MapBool(MoveForwards, keyboardDeviceId, gainput::KeyW);
		map->MapBool(MoveBackwards, keyboardDeviceId, gainput::KeyS);
		map->MapBool(MoveUp, keyboardDeviceId, gainput::KeySpace);
		map->MapBool(MoveDown, keyboardDeviceId, gainput::KeyShiftL);
		map->MapBool(RotateLeft, keyboardDeviceId, gainput::KeyLeft);
		map->MapBool(RotateRight, keyboardDeviceId, gainput::KeyRight);
		map->MapBool(RotateUp, keyboardDeviceId, gainput::KeyUp);
		map->MapBool(RotateDown, keyboardDeviceId, gainput::KeyDown);
		map->MapBool(EnableMouseRotation, mouseDeviceId, gainput::MouseButtonRight);
		map->MapFloat(MouseX, mouseDeviceId, gainput::MouseAxisX);
		map->MapFloat(MouseY, mouseDeviceId, gainput::MouseAxisY);
	}

	EngineImGui::~EngineImGui() {
		renderer.lock()->PostRenderComponentsRenderEvent -= Sharp::EventHandler::Bind(this, &EngineImGui::Render);
	}

	void EngineImGui::Render() {
		if (allowCameraMovement)
			CameraMovement();

		const gainput::InputDevice* keyboard = inputManager.lock()->GetInputManager().GetDevice(
			inputManager.lock()->GetKeyboardId());

		if (keyboard->GetBool(gainput::KeyCtrlL) && keyboard->GetBool(gainput::KeyShiftL) && keyboard->GetBool(gainput::KeyE)) {
			if (devMenuCooldown < 0.f) {
				showMenuBar = !showMenuBar;
				devMenuCooldown = 0.25f;
			}
		}
		if (devMenuCooldown >= 0.f)
			devMenuCooldown -= Time::Get()->GetDeltaTime();

		if (showMenuBar) DevMenu();
	}

	void EngineImGui::DevMenu() {
		ImGui::Begin("Menubar demo", &open, menuBarWindowFlags);

		ImGui::BeginMenuBar();
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::BeginMenu("Engine")) {
				if (ImGui::MenuItem("Close game"))
					Window::Get()->SetShouldClose(true);
				if (ImGui::MenuItem("Toggle Camera Movement"))
					allowCameraMovement = !allowCameraMovement;
				if (ImGui::MenuItem("Toggle Play/Pause mode"))
					Application::SetIsPlaying(!Application::GetIsPlaying());
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
		ImGui::End();

		ImGui::SetNextWindowSize(ImVec2(175, 50));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 175, 0));
		ImGui::Begin("FPS window", &open, fpsWindowFlags);

		const std::shared_ptr<Time> time = Time::Get();
		const float frameCount = 1.f / time->GetDeltaTime();
		if (++currentIterationIndex >= maxIterations) {
			previousFramerates.erase(previousFramerates.begin());
			previousFramerates.push_back(frameCount);
		} else
			previousFramerates[currentIterationIndex] = frameCount;

		ImGui::PlotHistogram("FPS", previousFramerates.data(), maxIterations, 0, std::to_string(static_cast<int>(frameCount)).c_str());

		ImGui::End();
	}

	void EngineImGui::CameraMovement() {
		// TODO: Replace this with scene view window
		if (ImGui::GetIO().WantTextInput) return;

		constexpr float cameraMovementSpeed = 100.f;
		constexpr float cameraRotationSpeed = 1.f;
		constexpr float cameraMouseRotationSpeed = 100.f;

		if (activeCamera.expired() || !activeCamera.lock()->GetIsActiveAndEnabled()) {
			const std::vector<std::shared_ptr<CameraComponent>> allActiveCameras = CameraManager::Get()->GetAllActiveCameras();
			if (allActiveCameras.empty()) return;

			activeCamera = allActiveCameras[0];
		}

		const std::shared_ptr<CameraComponent> lockedCameraComponent = activeCamera.lock();
		const std::shared_ptr<TransformComponent> lockedCameraTransformComponent = lockedCameraComponent->GetComponent<TransformComponent>();
		const float deltaTime = Time::Get()->GetDeltaTime();
		const float deltaMovementSpeed = cameraMovementSpeed * deltaTime;
		const float deltaRotationSpeed = cameraRotationSpeed * deltaTime;
		const float deltaMouseRotationSpeed = cameraMouseRotationSpeed * deltaTime;

		if (map->GetBool(MoveForwards))
			lockedCameraTransformComponent->Translate(0, 0, deltaMovementSpeed);
		if (map->GetBool(MoveBackwards))
			lockedCameraTransformComponent->Translate(0, 0, -deltaMovementSpeed);
		if (map->GetBool(MoveLeft))
			lockedCameraTransformComponent->Translate(deltaMovementSpeed);
		if (map->GetBool(MoveRight))
			lockedCameraTransformComponent->Translate(-deltaMovementSpeed);
		if (map->GetBool(MoveUp))
			lockedCameraTransformComponent->Translate(0, deltaMovementSpeed);
		if (map->GetBool(MoveDown))
			lockedCameraTransformComponent->Translate(0, -deltaMovementSpeed);
		if (map->GetBool(RotateLeft))
			lockedCameraTransformComponent->Rotate(glm::vec3(0.f, deltaRotationSpeed, 0.f));
		if (map->GetBool(RotateRight))
			lockedCameraTransformComponent->Rotate(glm::vec3(0.f, -deltaRotationSpeed, 0.f));
		if (map->GetBool(RotateUp))
			lockedCameraTransformComponent->Rotate(glm::vec3(-deltaRotationSpeed, 0.f, 0.f));
		if (map->GetBool(RotateDown))
			lockedCameraTransformComponent->Rotate(glm::vec3(deltaRotationSpeed, 0.f, 0.f));
		if (map->GetBool(EnableMouseRotation)) {
			lockedCameraTransformComponent->Rotate(
				glm::vec3(
					// TODO: Figure out why I need to flip Y and X axis here?
					map->GetFloatDelta(MouseY) * deltaMouseRotationSpeed,
					-map->GetFloatDelta(MouseX) * deltaMouseRotationSpeed,
					0.f));
		}
	}
} //namespace Engine