#pragma once

#include "Engine/Material/Material.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"

namespace Engine {
	class MaterialAssetImporter : public AssetImporter<Material> {
		friend class AssetManager;

		explicit MaterialAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~MaterialAssetImporter() override = default;
		MaterialAssetImporter(const MaterialAssetImporter& other) = delete;
		MaterialAssetImporter(MaterialAssetImporter&& other) = delete;
		MaterialAssetImporter& operator=(const MaterialAssetImporter& other) = delete;
		MaterialAssetImporter& operator=(MaterialAssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const override;
	};
}

