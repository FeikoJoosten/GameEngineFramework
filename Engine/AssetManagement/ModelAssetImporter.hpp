#pragma once

#include "Engine/Model/Model.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <map>

namespace Engine {
	class Entity;
	class Mesh;
	class ModelAssetImportSettings;
	class TransformComponent;

	class ModelAssetImporter : public AssetImporter<Model> {
		friend class AssetManager;

		explicit ModelAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

		Assimp::Importer importer;

	public:
		virtual ~ModelAssetImporter() override = default;
		ModelAssetImporter(const ModelAssetImporter& other) = delete;
		ModelAssetImporter(ModelAssetImporter&& other) = delete;
		ModelAssetImporter& operator=(const ModelAssetImporter& other) = delete;
		ModelAssetImporter& operator=(ModelAssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		virtual bool SupportsFile(const std::string& fullPath, const std::string& fileExtension) override;

		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings) override;

		virtual std::shared_ptr<Asset> ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) override;

		[[nodiscard]] virtual std::shared_ptr<Asset> LoadAsset(const xg::Guid& assetGuid) const override;

		virtual std::shared_ptr<AssetImportSettings> GetDefaultImportSettings() override;

	private:
		static void ProcessNode(const aiNode* node, const std::map<unsigned int, std::shared_ptr<Mesh>>& meshes, std::vector<std::shared_ptr<Entity>>& entities, const::std::shared_ptr<TransformComponent>& parent, const std::shared_ptr<ModelAssetImportSettings>& importSettings);
	};
}

