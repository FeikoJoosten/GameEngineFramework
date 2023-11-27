﻿#pragma once

#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"

namespace Engine {
	class AssetAssetImporter : public AssetImporter<Asset> {
		friend class AssetManager;

		explicit AssetAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~AssetAssetImporter() override = default;
		AssetAssetImporter(const AssetAssetImporter& other) = delete;
		AssetAssetImporter(AssetAssetImporter&& other) = delete;
		AssetAssetImporter& operator=(const AssetAssetImporter& other) = delete;
		AssetAssetImporter& operator=(AssetAssetImporter&& other) = delete;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const override;
	};
}