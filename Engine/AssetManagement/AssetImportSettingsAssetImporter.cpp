#include "Engine/AssetManagement/AssetImportSettingsAssetImporter.hpp"

namespace Engine {
	AssetImportSettingsAssetImporter::AssetImportSettingsAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<AssetImportSettings>(assetRegistry) {}

	const char* AssetImportSettingsAssetImporter::GetDefaultAssetExtension() const {
		return ".importSettings";
	}

	std::shared_ptr<Asset> AssetImportSettingsAssetImporter::CreateAsset() const {
		return std::make_shared<AssetImportSettings>();
	}
}
