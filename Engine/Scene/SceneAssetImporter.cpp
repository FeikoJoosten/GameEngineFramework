#include "Engine/Scene/SceneAssetImporter.hpp"
#include "Engine/Scene/Scene.hpp"

namespace Engine {
	SceneAssetImporter::SceneAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Scene>(assetRegistry) {}

	const char* SceneAssetImporter::GetDefaultAssetExtension() const {
		return ".scene";
	}

	std::shared_ptr<Asset> SceneAssetImporter::CreateAsset() const {
		return std::shared_ptr<Scene>(new Scene());
	}
}
