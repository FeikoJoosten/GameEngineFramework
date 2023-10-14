#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetImportSettings.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/types/base_class.hpp>
#include <assimp/postprocess.h>

namespace Engine {
	class ENGINE_LOCAL ModelAssetImportSettings : public AssetImportSettings {
		friend cereal::access;

	public:
		inline static const std::string EXTENSION = ".importSettings";
		unsigned importFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes | aiProcess_RemoveComponent | aiProcess_RemoveRedundantMaterials;
		float scaleFactor = 0.01f;

		explicit ModelAssetImportSettings() = default;
		virtual ~ModelAssetImportSettings() override = default;
		ModelAssetImportSettings(const ModelAssetImportSettings& other) = delete;
		ModelAssetImportSettings(ModelAssetImportSettings&& other) = delete;
		ModelAssetImportSettings& operator=(const ModelAssetImportSettings& other) = delete;
		ModelAssetImportSettings& operator=(ModelAssetImportSettings&& other) = delete;

	private:

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void ModelAssetImportSettings::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<AssetImportSettings>(this)),
			CEREAL_NVP(importFlags),
			CEREAL_NVP(scaleFactor)
		);
	}
}
