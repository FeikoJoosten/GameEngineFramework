#pragma once

#include "Engine/Api/Api.hpp"

#include <memory>
#include <string>

namespace Engine {
	class Asset;
	class AssetImportSettings;

	class ENGINE_API IAssetImporter {
	public:
		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const = 0;

		[[nodiscard]] virtual bool SupportsAsset(const std::shared_ptr<Asset>& assetToVerify) = 0;

		[[nodiscard]] virtual bool SupportsType(const type_info& typeInfo) = 0;

		[[nodiscard]] virtual bool SupportsFile(const std::string& fullPath, const std::string& fileExtension) = 0;

		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport) = 0;

		virtual std::shared_ptr<Asset> ImportAsset(const std::string& fullPath, const char* assetNameWithExtension) = 0;

		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::string& fullPath, const char* assetNameWithExtension) = 0;

		[[nodiscard]] virtual std::shared_ptr<Asset> LoadAsset(const xg::Guid& assetGuid) const = 0;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const = 0;

		[[nodiscard]] virtual std::shared_ptr<AssetImportSettings> GetDefaultImportSettings() const = 0;
	};
}
