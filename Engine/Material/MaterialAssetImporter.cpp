#include "Engine/Material/MaterialAssetImporter.hpp"

namespace Engine {

	MaterialAssetImporter::MaterialAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Material>(assetRegistry){}

	const char* MaterialAssetImporter::GetDefaultAssetExtension() const {
		return ".material";
	}

	std::shared_ptr<Asset> MaterialAssetImporter::CreateAsset() const {
		return std::shared_ptr<Material>(new Material());
	}
}
