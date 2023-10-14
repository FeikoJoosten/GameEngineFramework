#include "Engine/AssetManagement/AssetImportSettingsAssetImporter.hpp"

namespace Engine {
	AssetImportSettingsAssetImporter::AssetImportSettingsAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<AssetImportSettings>(assetRegistry) {}

	const char* AssetImportSettingsAssetImporter::GetDefaultAssetExtension() const {
		return ".importSettings";
	}

	std::shared_ptr<Asset> AssetImportSettingsAssetImporter::ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return AssetManager::Get()->ReadDataFromPath<std::shared_ptr<Asset>>(pathInProject, assetNameWithExtension);
	}
}
