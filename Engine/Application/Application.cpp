#include "Engine/Application/Application.hpp"
#include "Engine/Camera/CameraComponent.hpp"
#include "Engine/Camera/CameraManager.hpp"
#include "Engine/Collision/CollisionSystem.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Utility/Time.hpp"

namespace Engine {
	static bool isPlaying = false;

	bool Application::GetIsPlaying() {
		return isPlaying;
	}

	void Application::SetIsPlaying(const bool newIsPlaying) {
		isPlaying = newIsPlaying;
	}

	void Application::Update() {

		const std::shared_ptr<Time> time = Time::Get();
		time->OnUpdateBegin();

		const std::shared_ptr<Engine> engine = Engine::instance;

		// TODO: Replace with something on application level which controls the playing state
		if (isPlaying) {
			if (engine->entitySystem != nullptr)
				engine->entitySystem->Update();

			if (engine->collisionSystem != nullptr) {
				if (engine->collisionSystem->IsRunning() == false)
					engine->collisionSystem->Start();
			}

			if (engine->collisionSystem != nullptr)
				engine->collisionSystem->Update();
		}

		//Input manager should always update. 
		if (engine->inputManager != nullptr)
			engine->inputManager->Update();

		Render();
		time->OnUpdateEnd();
	}

	void Application::Quit() {
	}

	void Application::Render() {
		const std::shared_ptr<Engine> engine = Engine::instance;

		if (engine->renderer == nullptr) return;
		if (engine->cameraManager == nullptr) return;

		const std::vector<std::shared_ptr<CameraComponent>> allCameras = engine->cameraManager->GetAllActiveCameras();

		for (const std::shared_ptr<CameraComponent>& camera : allCameras) {
			engine->renderer->RenderFrame(
				camera->GetView(),
				camera->GetProjection());
		}
	}
}
