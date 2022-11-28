#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Model/Model.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Engine {
	class ENGINE_API ModelImporter : public AssetImporter<Model> {
		friend class AssetManager;

		explicit ModelImporter(std::shared_ptr<AssetRegistry> assetRegistry, unsigned importFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes);

	public:
		virtual ~ModelImporter() override = default;
		ModelImporter(const ModelImporter& other) = delete;
		ModelImporter(ModelImporter&& other) = delete;
		ModelImporter& operator=(const ModelImporter& other) = delete;
		ModelImporter& operator=(ModelImporter&& other) = delete;

		virtual bool SupportsFileExtension(const std::string& fileExtension) override;

		virtual std::shared_ptr<Asset> ProcessAsset(const std::string& pathInProject, const std::string& assetName) override;

		virtual std::shared_ptr<Model> LoadAsset(xg::Guid assetId) override;

		virtual std::shared_ptr<Model> LoadAsset(const std::string& pathInProject, const std::string& assetName) override;

		void SetImportFlags(unsigned newImportFlags);

	private:
		unsigned importFlags {};
		Assimp::Importer importer;
	};
}

