#include "Engine/Scene/SceneManager.hpp"

#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/SceneImporter.hpp"
#include "Engine/Engine/Engine.hpp"

namespace Engine {

	std::shared_ptr<SceneManager> SceneManager::Get() {
		return Engine::GetSceneManager();
	}

	std::vector<std::shared_ptr<Scene>> SceneManager::GetOpenScenes() {
		return openScenes;
	}

	std::shared_ptr<Scene> SceneManager::OpenScene(const std::string& pathInProject, const std::string& sceneName) {
		std::shared_ptr<Scene> openedScene = AssetManager::Get()->GetAssetImporter<SceneImporter>()->LoadAsset(pathInProject, sceneName + Scene::extension);

		if(!openedScene) {
			DEBUG_WARNING("Not opening scene, because no scene can be found!");
			return {};
		}

		OpenScene(openedScene);
		return openedScene;
	}

	void SceneManager::OpenScene(const std::shared_ptr<Scene>& sceneToOpen) {
		if(IsSceneOpened(sceneToOpen)) {
			DEBUG_WARNING("Not opening scene, because the scene " + sceneToOpen->GetName() + "is already open");
			return;
		}

		openScenes.push_back(sceneToOpen);
		OnSceneOpenedEvent(sceneToOpen);
	}

	void SceneManager::CloseScene(const std::shared_ptr<Scene>& sceneToClose) {
		if (!IsSceneOpened(sceneToClose)) return; // No need to log

		openScenes.erase(std::find(openScenes.begin(), openScenes.end(), sceneToClose));
		OnSceneClosedEvent(sceneToClose);
	}

	bool SceneManager::IsSceneOpened(const std::shared_ptr<Scene>& sceneToCheck) {
		return std::find(openScenes.begin(), openScenes.end(), sceneToCheck) != openScenes.end();
	}

	std::shared_ptr<Scene> SceneManager::CreateScene(const std::string& sceneName, const std::string& pathInProject) {
		std::shared_ptr<Scene> createdScene = std::shared_ptr<Scene>(new Scene(sceneName));

		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		assetManager->WriteDataToPath(pathInProject, createdScene);

		return createdScene;
	}
}
