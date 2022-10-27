#include "Engine/AssetManagement/MeshAssetImporter.hpp"

namespace Engine {

	bool MeshAssetImporter::SupportsFileExtension(const char* fileExtension) {
		return false;
	}

	void MeshAssetImporter::ProcessAsset(const char* fullSystemPath) {
		
	}

	std::shared_ptr<Mesh> MeshAssetImporter::LoadAsset(xg::Guid assetId) {
		return {};
	}
}
