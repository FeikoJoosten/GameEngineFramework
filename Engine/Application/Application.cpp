#include "Engine/Application/Application.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Components/CameraComponent.hpp"
#include "Engine/Camera/CameraManager.hpp"
#include "Engine/Engine/Engine.hpp"
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

		if (engine->assetManager != nullptr)
			engine->assetManager->UpdateFileWatcher();

		// TODO: Replace with something on application level which controls the playing state
		if (isPlaying) {
			if (engine->entitySystem != nullptr)
				engine->entitySystem->Update();
		}

		//Input manager should always update. 
		if (engine->inputManager != nullptr)
			engine->inputManager->Update();

		if (engine->renderer && engine->cameraManager ){
			for (const std::vector<std::shared_ptr<CameraComponent>> allCameras = engine->cameraManager->GetAllActiveCameras();
				const std::shared_ptr<CameraComponent>& camera : allCameras)
				engine->renderer->RenderFrame(camera->GetView(), camera->GetProjection());
		}
		
		time->OnUpdateEnd();
	}

	void Application::Quit() {
	}
}
