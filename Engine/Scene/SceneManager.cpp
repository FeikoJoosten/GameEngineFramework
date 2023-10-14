#include "Engine/Scene/SceneManager.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/SceneAssetImporter.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Scene/Scene.hpp"

namespace Engine {

	std::shared_ptr<SceneManager> SceneManager::Get() {
		return Engine::GetSceneManager();
	}

	std::vector<std::shared_ptr<Scene>> SceneManager::GetOpenScenes() {
		return openScenes;
	}

	std::shared_ptr<Scene> SceneManager::OpenScene(const std::string& pathInProject, const std::string& sceneName) {
		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		xg::Guid assetGuid;
		if (!AssetRegistry::TryGetGuidForPath(pathInProject, sceneName.c_str(), assetGuid)) {
			DEBUG_ERROR("Cannot find asset for guid");
			return {};
		}
		std::shared_ptr<Scene> openedScene = assetManager->LoadAsset<Scene>(assetGuid);

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

		const std::shared_ptr<EntitySystem> entitySystem = EntitySystem::Get();
		for (const std::shared_ptr<Entity>& entity : sceneToOpen->GetAllEntities())
			entitySystem->AddEntity(entity);

		openScenes.push_back(sceneToOpen);
		OnSceneOpenedEvent(sceneToOpen);
	}

	void SceneManager::CloseScene(const std::shared_ptr<Scene>& sceneToClose) {
		if (!IsSceneOpened(sceneToClose)) return; // No need to log


		const std::shared_ptr<EntitySystem> entitySystem = EntitySystem::Get();
		for (const std::shared_ptr<Entity>& entity : sceneToClose->GetAllEntities())
			entitySystem->RemoveEntity(entity);

		openScenes.erase(std::ranges::find(openScenes, sceneToClose));
		OnSceneClosedEvent(sceneToClose);
	}

	bool SceneManager::IsSceneOpened(const std::shared_ptr<Scene>& sceneToCheck) {
		return std::ranges::find(openScenes, sceneToCheck) != openScenes.end();
	}

	std::shared_ptr<Scene> SceneManager::CreateScene(const std::string& sceneName, const std::string& pathInProject) {
		std::shared_ptr<Scene> createdScene = std::shared_ptr<Scene>(new Scene(sceneName));

		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		const std::shared_ptr<SceneAssetImporter> sceneAssetImporter = assetManager->GetAssetImporter<SceneAssetImporter>();
		assetManager->GetAssetRegistry()->TryRegisterAsset(createdScene, pathInProject, (sceneName + sceneAssetImporter->GetDefaultAssetExtension()).c_str());

		return createdScene;
	}
}
