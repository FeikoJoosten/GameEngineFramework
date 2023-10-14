#pragma once

#include "Engine/Texture/Texture.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"

namespace Engine {
	class TextureAssetImporter : public AssetImporter<Texture> {
		friend class AssetManager;

		explicit TextureAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry);

	public:
		virtual ~TextureAssetImporter() override = default;
		TextureAssetImporter(const TextureAssetImporter& other) = delete;
		TextureAssetImporter(TextureAssetImporter&& other) = delete;
		TextureAssetImporter& operator=(const TextureAssetImporter& other) = delete;
		TextureAssetImporter& operator=(TextureAssetImporter&& other) = delete;

		virtual bool SupportsFile(const std::string& fullPath, const std::string& fileExtension) override;

		virtual std::shared_ptr<Asset> ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) override;

		static std::shared_ptr<Texture> ProcessRawData(const unsigned char* data, int size, int& width, int& height, int& channels, int desiredChannels);
	};
}

