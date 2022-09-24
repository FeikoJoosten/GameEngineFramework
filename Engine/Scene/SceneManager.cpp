#include "Engine/Scene/SceneManager.hpp"

#include "Engine/Engine/Engine.hpp"

namespace Engine {

	SceneManager::SceneManager() {
		
	}

	SceneManager::~SceneManager() {
		
	}

	std::shared_ptr<SceneManager> SceneManager::Get() {
		return Engine::GetSceneManager();
	}

	std::shared_ptr<Scene> SceneManager::GetActiveScene() {
		return activeScene;
	}

	void SceneManager::OpenScene(const std::string& path) {
		
	}

	void SceneManager::OpenScene(std::shared_ptr<Scene> scene) {
		
	}
}
