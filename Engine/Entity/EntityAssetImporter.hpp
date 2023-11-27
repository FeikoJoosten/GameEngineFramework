#pragma once

#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/Entity/Entity.hpp"

namespace Engine {
	class EntityAssetImporter : public AssetImporter<Entity> {
		friend class AssetManager;

		explicit EntityAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~EntityAssetImporter() override = default;
		EntityAssetImporter(const EntityAssetImporter& other) = delete;
		EntityAssetImporter(EntityAssetImporter&& other) = delete;
		EntityAssetImporter& operator=(const EntityAssetImporter& other) = delete;
		EntityAssetImporter& operator=(EntityAssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		[[nodiscard]] virtual std::shared_ptr<Asset> CreateAsset() const override;
	};
}

