#include "Engine/AssetManagement/SceneImporter.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Scene/Scene.hpp"

namespace Engine {

	SceneImporter::SceneImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Scene>(assetRegistry) {}

	bool SceneImporter::SupportsFileExtension(const std::string& fileExtension) {
		return fileExtension == Scene::extension;
	}

	std::shared_ptr<Asset> SceneImporter::ProcessAsset(const std::string& fullPath, const std::string& assetName) {
		return AssetManager::Get()->ReadDataFromFullPath<std::shared_ptr<Asset>>(fullPath, assetName);
	}

	std::shared_ptr<Scene> SceneImporter::LoadAsset(xg::Guid assetId) {
		std::string pathInProject;
		std::string assetName;
		if (!assetRegistry->TryGetPathForGuid(assetId, pathInProject, assetName)) {
			DEBUG_ERROR("Failed to load scene, because no scene is known with GUID: " + assetId.str());
			return {};
		}

		return LoadAsset(pathInProject, assetName);
	}

	std::shared_ptr<Scene> SceneImporter::LoadAsset(const std::string& pathInProject, const std::string& assetName) {
		// If we load as the Scene class, cereal throws an exception when loading as it cannot upcast from Scene --> Asset?
		return std::static_pointer_cast<Scene>(AssetManager::Get()->ReadDataFromPath<std::shared_ptr<Asset>>(pathInProject, assetName));
	}
}
