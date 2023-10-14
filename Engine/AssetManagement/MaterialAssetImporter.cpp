#include "Engine/AssetManagement/MaterialAssetImporter.hpp"

namespace Engine {

	MaterialAssetImporter::MaterialAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Material>(assetRegistry){}

	const char* MaterialAssetImporter::GetDefaultAssetExtension() const {
		return ".material";
	}

	std::shared_ptr<Asset> MaterialAssetImporter::ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return {};
	}
}
