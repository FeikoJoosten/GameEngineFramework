#pragma once

#include "Engine/AssetManagement/AssetImporter.hpp"

namespace Engine {
	class Scene;

	class SceneImporter : public AssetImporter<Scene> {
		friend class AssetManager;

		explicit SceneImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~SceneImporter() override = default;
		SceneImporter(const SceneImporter& other) = delete;
		SceneImporter(SceneImporter&& other) = delete;
		SceneImporter& operator=(const SceneImporter& other) = delete;
		SceneImporter& operator=(SceneImporter&& other) = delete;

		virtual bool SupportsFileExtension(const std::string& fileExtension) override;

		virtual std::shared_ptr<Asset> ProcessAsset(const std::string& fullPath, const std::string& assetName) override;

		virtual std::shared_ptr<Scene> LoadAsset(xg::Guid assetId) override;

		virtual std::shared_ptr<Scene> LoadAsset(const std::string& pathInProject, const std::string& assetName) override;
	};
}

