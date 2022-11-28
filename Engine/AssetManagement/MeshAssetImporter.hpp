#pragma once

#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/Mesh/Mesh.hpp"

#include <crossguid/guid.hpp>

namespace Engine {
	//class ENGINE_API MeshAssetImporter : public AssetImporter<Mesh> {
	//	friend class AssetManager;
	//
	//	explicit MeshAssetImporter() = default;
	//
	//public:
	//	virtual ~MeshAssetImporter() override = default;
	//	MeshAssetImporter(const MeshAssetImporter& other) = delete;
	//	MeshAssetImporter(MeshAssetImporter&& other) = delete;
	//	MeshAssetImporter& operator=(const MeshAssetImporter& other) = delete;
	//	MeshAssetImporter& operator=(MeshAssetImporter&& other) = delete;
	//
	//	virtual bool SupportsFileExtension(const char* fileExtension) override;
	//
	//	virtual std::shared_ptr<Asset> ProcessAsset(const std::string& fullPath, const std::string& assetName) override;
	//
	//	virtual std::shared_ptr<Mesh> LoadAsset(xg::Guid assetId) override;
	//};
}

