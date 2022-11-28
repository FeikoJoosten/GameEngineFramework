#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"

#include <memory>

namespace Engine {
	template <class AssetType>
	class ENGINE_API AssetImporter : public IAssetImporter {
	protected:
		std::shared_ptr<AssetRegistry> assetRegistry;

		AssetImporter(std::shared_ptr<AssetRegistry> assetRegistry);

	public:
		AssetImporter() = delete;
		virtual ~AssetImporter() = default;
		AssetImporter(const AssetImporter& other) = delete;
		AssetImporter(AssetImporter&& other) = delete;
		AssetImporter& operator=(const AssetImporter& other) = delete;
		AssetImporter& operator=(AssetImporter&& other) = delete;

		virtual std::shared_ptr<AssetType> LoadAsset(xg::Guid assetId) = 0;

		virtual std::shared_ptr<AssetType> LoadAsset(const std::string& pathInProject, const std::string& assetName) = 0;
	};

	template <class AssetType> AssetImporter<AssetType>::AssetImporter(std::shared_ptr<AssetRegistry> assetRegistry) :assetRegistry(assetRegistry) {
		static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");
	}
}

