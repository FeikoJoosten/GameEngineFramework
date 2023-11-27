#pragma once

#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/Mesh/Mesh.hpp"

#include <assimp/mesh.h>

namespace Engine {
	class MeshAssetImporter : public AssetImporter<Mesh> {
		friend class AssetManager;
	
		explicit MeshAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);
	
	public:
		virtual ~MeshAssetImporter() override = default;
		MeshAssetImporter(const MeshAssetImporter& other) = delete;
		MeshAssetImporter(MeshAssetImporter&& other) = delete;
		MeshAssetImporter& operator=(const MeshAssetImporter& other) = delete;
		MeshAssetImporter& operator=(MeshAssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const override;
	
		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings) override;

		[[nodiscard]] virtual std::shared_ptr<Asset> LoadAsset(const xg::Guid& assetGuid, const std::shared_ptr<AssetImportSettings>& importSettings) const override;

		static void ProcessRawData(const std::shared_ptr<Mesh>& relatedMesh, const aiMesh* assimpMesh);
	};
}

