#include "Engine/AssetManagement/SceneAssetImporter.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Scene/Scene.hpp"

namespace Engine {
	SceneAssetImporter::SceneAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Scene>(assetRegistry) {}

	const char* SceneAssetImporter::GetDefaultAssetExtension() const {
		return ".scene";
	}

	std::shared_ptr<Asset> SceneAssetImporter::ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return AssetManager::Get()->ReadDataFromFullPath<std::shared_ptr<Asset>>(pathInProject, assetNameWithExtension);
	}
}
