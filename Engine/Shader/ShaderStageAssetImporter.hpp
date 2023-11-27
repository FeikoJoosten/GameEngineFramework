#pragma once

#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/Shader/ShaderStage.hpp"

namespace Engine {
	class ShaderStageAssetImporter : public AssetImporter<ShaderStage> {
		friend class AssetManager;

		explicit ShaderStageAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~ShaderStageAssetImporter() override = default;
		ShaderStageAssetImporter(const ShaderStageAssetImporter& other) = delete;
		ShaderStageAssetImporter(ShaderStageAssetImporter&& other) = delete;
		ShaderStageAssetImporter& operator=(const ShaderStageAssetImporter& other) = delete;
		ShaderStageAssetImporter& operator=(ShaderStageAssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const override;
	};
}

