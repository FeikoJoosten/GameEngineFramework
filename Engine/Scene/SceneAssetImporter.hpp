#pragma once

#include "Engine/Scene/Scene.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"

namespace Engine {
	class SceneAssetImporter : public AssetImporter<Scene> {
		friend class AssetManager;

		explicit SceneAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~SceneAssetImporter() override = default;
		SceneAssetImporter(const SceneAssetImporter& other) = delete;
		SceneAssetImporter(SceneAssetImporter&& other) = delete;
		SceneAssetImporter& operator=(const SceneAssetImporter& other) = delete;
		SceneAssetImporter& operator=(SceneAssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const override;
	};
}

