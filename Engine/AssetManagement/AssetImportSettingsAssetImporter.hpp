#pragma once

#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/AssetManagement/AssetImportSettings.hpp"

namespace Engine {
	class AssetImportSettingsAssetImporter : public AssetImporter<AssetImportSettings> {
		friend class AssetManager;

		explicit AssetImportSettingsAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~AssetImportSettingsAssetImporter() override = default;
		AssetImportSettingsAssetImporter(const AssetImportSettingsAssetImporter& other) = delete;
		AssetImportSettingsAssetImporter(AssetImportSettingsAssetImporter&& other) = delete;
		AssetImportSettingsAssetImporter& operator=(const AssetImportSettingsAssetImporter& other) = delete;
		AssetImportSettingsAssetImporter& operator=(AssetImportSettingsAssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const override;
	};
}