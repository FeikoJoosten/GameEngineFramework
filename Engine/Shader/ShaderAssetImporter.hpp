#pragma once

#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/Shader/Shader.hpp"

namespace Engine {
	class ShaderAssetImporter : public AssetImporter<Shader> {
		friend class AssetManager;

		explicit ShaderAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~ShaderAssetImporter() override = default;
		ShaderAssetImporter(const ShaderAssetImporter& other) = delete;
		ShaderAssetImporter(ShaderAssetImporter&& other) = delete;
		ShaderAssetImporter& operator=(const ShaderAssetImporter& other) = delete;
		ShaderAssetImporter& operator=(ShaderAssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const override;

		[[nodiscard]] virtual std::shared_ptr<Asset> LoadAsset(const xg::Guid& assetGuid, const std::shared_ptr<AssetImportSettings>& importSettings) const override;
	};
}

