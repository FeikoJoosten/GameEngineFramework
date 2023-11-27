#pragma once


#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetImportSettings.hpp"
#include "Engine/AssetManagement/AssetReference.hpp"
#include "Engine/Utility/Defines.hpp"

#include <assimp/postprocess.h>
#include <cereal/types/base_class.hpp>
#include <nameof.hpp>

namespace Engine {
	class ENGINE_LOCAL ModelAssetImportSettings : public AssetImportSettings {
		friend cereal::access;
		friend class ModelAssetImporter;

	public:
		unsigned importFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials;
		float scaleFactor = 0.01f;
		AssetReference<AssetImportSettings> textureAssetImportSettings;
		AssetReference<AssetImportSettings> meshAssetImportSettings;

		explicit ModelAssetImportSettings();
		virtual ~ModelAssetImportSettings() override = default;
		ModelAssetImportSettings(const ModelAssetImportSettings& other) = delete;
		ModelAssetImportSettings(ModelAssetImportSettings&& other) = delete;
		ModelAssetImportSettings& operator=(const ModelAssetImportSettings& other) = delete;
		ModelAssetImportSettings& operator=(ModelAssetImportSettings&& other) = delete;

	private:

		template<class Archive>
		void Save(Archive& archive) const;

		template<class Archive>
		void Load(Archive& archive);
	};

	template <class Archive> void ModelAssetImportSettings::Save(Archive& archive) const {
		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		const std::shared_ptr<AssetRegistry>& assetRegistry = assetManager->GetAssetRegistry();
		const std::string& assetRegistryPath = AssetRegistry::GetAssetRegistryPathForGuid(GetGuid());
		if (!AssetRegistry::IsAssetRegistered(textureAssetImportSettings)) {
			const std::shared_ptr<AssetImportSettings> textureAssetImporterSettingsPointer = textureAssetImportSettings;
			textureAssetImporterSettingsPointer->SetName("textureImportSettings");
			assetManager->WriteAssetToFullPath(assetRegistryPath, textureAssetImporterSettingsPointer);
		}
		if (!AssetRegistry::IsAssetRegistered(meshAssetImportSettings)) {
			const std::shared_ptr<AssetImportSettings> meshAssetImporterSettingsPointer = meshAssetImportSettings;
			meshAssetImporterSettingsPointer->SetName("meshImportSettings");
			assetManager->WriteAssetToFullPath(assetRegistryPath, meshAssetImporterSettingsPointer);
		}

		archive(
			CEREAL_NVP(cereal::base_class<AssetImportSettings>(this)),
			CEREAL_NVP(importFlags),
			CEREAL_NVP(scaleFactor),
			CEREAL_NVP(textureAssetImportSettings),
			CEREAL_NVP(meshAssetImportSettings)
		);
	}

	template <class Archive>
	void ModelAssetImportSettings::Load(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<AssetImportSettings>(this)),
			CEREAL_NVP(importFlags),
			CEREAL_NVP(scaleFactor),
			CEREAL_NVP(textureAssetImportSettings),
			CEREAL_NVP(meshAssetImportSettings)
		);
	}
}
