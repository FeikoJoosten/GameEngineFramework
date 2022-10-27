#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"

#include <memory>

namespace Engine {
	class ENGINE_API IAssetImporter {
	public:
		virtual bool SupportsFileExtension(const char* fileExtension) = 0;

		virtual std::shared_ptr<Asset> ProcessAsset(const char* fullSystemPath) = 0;
	};

	template <class AssetType>
	class ENGINE_LOCAL AssetImporter : public IAssetImporter {
	protected:
		AssetImporter();

	public:
		virtual ~AssetImporter() = default;
		AssetImporter(const AssetImporter& other) = delete;
		AssetImporter(AssetImporter&& other) = delete;
		AssetImporter& operator=(const AssetImporter& other) = delete;
		AssetImporter& operator=(AssetImporter&& other) = delete;

		virtual std::shared_ptr<AssetType> LoadAsset(xg::Guid assetId) = 0;
	};

	template <class AssetType>
	AssetImporter<AssetType>::AssetImporter() {
		static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");
	}
}

