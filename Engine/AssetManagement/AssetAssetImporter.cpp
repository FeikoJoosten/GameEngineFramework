#include "Engine/AssetManagement/AssetAssetImporter.hpp"

namespace Engine {
	AssetAssetImporter::AssetAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Asset>(assetRegistry) {}

	std::shared_ptr<Asset> AssetAssetImporter::CreateAsset() const {
		return std::shared_ptr<Asset>(new Asset());
	}
}
