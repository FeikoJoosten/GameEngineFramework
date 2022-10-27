#include "Engine/AssetManagement/MeshAssetImporter.hpp"

namespace Engine {

	bool MeshAssetImporter::SupportsFileExtension(const char* fileExtension) {
		return false;
	}

	std::shared_ptr<Asset> MeshAssetImporter::ProcessAsset(const char* fullSystemPath) {
		return {};
	}

	std::shared_ptr<Mesh> MeshAssetImporter::LoadAsset(xg::Guid assetId) {
		return {};
	}
}
