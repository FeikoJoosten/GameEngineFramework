#include "Engine/Texture/TextureAssetImporter.hpp"
#include "Engine/Texture/Texture.hpp"
#include "Engine/Texture/TextureAssetImportSettings.hpp"

#include <stb/stb_image.h>

namespace Engine {
	TextureAssetImporter::TextureAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Texture>(assetRegistry) {}

	bool TextureAssetImporter::SupportsFile(const std::string& fullPath, const std::string& fileExtension) {
		int width;
		int height;
		int channels;

		return stbi_info(fullPath.c_str(), &width, &height, &channels) == 1;
	}

	void TextureAssetImporter::ProcessRawData(const std::shared_ptr<Texture>& relatedTexture, const unsigned char* data, const int size, int& width, int& height, int& channels, const int desiredChannels) {
		if (!data) {
			DEBUG_ERROR("Invalid data provided!");
			return;
		}

		unsigned char* textureData = stbi_load_from_memory(data, size, &width, &height, &channels, desiredChannels);
		if (!textureData) {
			DEBUG_ERROR("Failed to load texture data!");
			return;
		}
		
		const int textureSize = width * height * desiredChannels;
		relatedTexture->textureData = std::vector(textureData, textureData + textureSize);
	}

	std::shared_ptr<Asset> TextureAssetImporter::CreateAsset() const {
		return std::shared_ptr<Texture>(new Texture());
	}

	std::shared_ptr<AssetImportSettings> TextureAssetImporter::GetDefaultImportSettings() const {
		return std::make_shared<TextureAssetImportSettings>();
	}

	std::shared_ptr<Asset> TextureAssetImporter::ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings) {
		const std::shared_ptr<Texture> textureToImport = std::dynamic_pointer_cast<Texture>(assetToImport);
		if (!textureToImport) {
			DEBUG_ERROR("The asset trying to import is not of type Texture!");
			return {};
		}

		if (!AssetRegistry::TryWriteCustomDataToAsset(textureToImport->GetGuid(), textureToImport, GetDefaultAssetExtension())) {
			DEBUG_ERROR("Failed to save imported Texture to disk!");
			return {};
		}

		return textureToImport;
	}

	std::shared_ptr<Asset> TextureAssetImporter::ImportAsset(const std::string& fullPath, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		const std::shared_ptr<TextureAssetImportSettings> textureAssetImportSettings = std::static_pointer_cast<TextureAssetImportSettings>(importSettings);

		xg::Guid relatedAssetGuid = importSettings->relatedAssetGuid;
		if (relatedAssetGuid.isValid() && !AssetRegistry::IsAssetRegistered(relatedAssetGuid))
			relatedAssetGuid = {}; // Reset the related asset guid since the related asset has been removed from the asset registry

		const std::shared_ptr<Texture> loadedTexture = std::shared_ptr<Texture>(new Texture());
		unsigned char* textureData = stbi_load((fullPath + "/" + assetNameWithExtension).c_str(), &loadedTexture->width, &loadedTexture->height, &loadedTexture->channels, 0);

		if (!textureData) {
			DEBUG_ERROR("Failed to load texture data!");
			return {};
		}

		if (!relatedAssetGuid.isValid() || !AssetRegistry::TryAssignGuidToAsset(loadedTexture, relatedAssetGuid))
			assetRegistry->TryRegisterAsset(loadedTexture, textureAssetImportSettings->GetGuid(), fullPath, assetNameWithExtension);

		const int textureSize = loadedTexture->width * loadedTexture->height * loadedTexture->channels;
		loadedTexture->textureData = std::vector(textureData, textureData + textureSize);
		if (!AssetRegistry::TryWriteCustomDataToAsset(loadedTexture->GetGuid(), loadedTexture, GetDefaultAssetExtension())) {
			DEBUG_ERROR("Failed to save imported texture to disk!");
			return {};
		}
		
		return loadedTexture;
	}

	std::shared_ptr<Asset> TextureAssetImporter::LoadAsset(const xg::Guid& assetGuid, const std::shared_ptr<AssetImportSettings>& importSettings) const {
		std::shared_ptr<Asset> loadedAsset;
		if (!AssetRegistry::TryLoadCustomDataFromAsset(assetGuid, GetDefaultAssetExtension(), loadedAsset)) return {};

		std::shared_ptr<Texture> loadedAssetAsTexture = std::static_pointer_cast<Texture>(loadedAsset);
		if (!loadedAssetAsTexture) {
			DEBUG_ERROR("Failed to load Texture!");
			return {};
		}

		std::string fullPath;
		std::string assetNameWithExtension;
		AssetRegistry::TryGetFullPathForGuid(assetGuid, fullPath, assetNameWithExtension);
		int width;
		int height;
		int channels;
		const unsigned char* textureData = stbi_load((fullPath + "/" + assetNameWithExtension).c_str(), &width, &height, &channels, 0);
		// TODO: Verify read width/height with saved texture width/height

		if (!textureData) {
			DEBUG_ERROR("Failed to load texture data!");
			return {};
		}

		const int textureSize = width * height * channels;
		loadedAssetAsTexture->textureData = std::vector(textureData, textureData + textureSize);

		return loadedAssetAsTexture;
	}
}
