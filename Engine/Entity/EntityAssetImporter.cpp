#include "Engine/Entity/EntityAssetImporter.hpp"

namespace Engine {

	EntityAssetImporter::EntityAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Entity>(assetRegistry) {}

	const char* EntityAssetImporter::GetDefaultAssetExtension() const {
		return ".entity";
	}

	std::shared_ptr<Asset> EntityAssetImporter::CreateAsset() const {
		return std::shared_ptr<Entity>(new Entity());
	}
}
