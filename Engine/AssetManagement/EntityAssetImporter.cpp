#include "Engine/AssetManagement/EntityAssetImporter.hpp"

namespace Engine {

	EntityAssetImporter::EntityAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Entity>(assetRegistry) {}

	const char* EntityAssetImporter::GetDefaultAssetExtension() const {
		return ".entity";
	}

	std::shared_ptr<Asset> EntityAssetImporter::ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return AssetManager::Get()->ReadDataFromFullPath<std::shared_ptr<Asset>>(pathInProject, assetNameWithExtension);
	}
}
