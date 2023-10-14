#include "Engine/AssetManagement/AssetAssetImporter.hpp"

namespace Engine {
	AssetAssetImporter::AssetAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Asset>(assetRegistry) {}

	std::shared_ptr<Asset> AssetAssetImporter::ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return AssetManager::Get()->ReadDataFromPath<std::shared_ptr<Asset>>(pathInProject, assetNameWithExtension);
	}
}
