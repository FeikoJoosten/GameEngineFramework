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
	
		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings) override;
	
		virtual std::shared_ptr<Asset> ImportAsset(const std::string& fullPath, const char* assetName, const std::shared_ptr<AssetImportSettings>& importSettings) override;

		[[nodiscard]] virtual std::shared_ptr<Asset> LoadAsset(const xg::Guid& assetGuid) const override;

		static std::shared_ptr<Mesh> ProcessRawData(const aiMesh* assimpMesh);
	};
}

