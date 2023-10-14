#include "Engine/AssetManagement/TextureAssetImporter.hpp"
#include "Engine/Texture/Texture.hpp"

#include <stb/stb_image.h>

namespace Engine {
	TextureAssetImporter::TextureAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Texture>(assetRegistry) {}

	bool TextureAssetImporter::SupportsFile(const std::string& fullPath, const std::string& fileExtension) {
		int width;
		int height;
		int channels;

		return stbi_info(fullPath.c_str(), &width, &height, &channels) == 1;
	}

	std::shared_ptr<Asset> TextureAssetImporter::ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		const std::shared_ptr<Texture> loadedTexture = std::shared_ptr<Texture>(new Texture());
		unsigned char* textureData = stbi_load((pathInProject + "/" + assetNameWithExtension).c_str(), &loadedTexture->width, &loadedTexture->height, &loadedTexture->channels, 0);

		if (!textureData) {
			DEBUG_ERROR("Failed to load texture data!");
		}

		loadedTexture->textureData = std::unique_ptr<unsigned char>(textureData);
		
		return loadedTexture;
	}

	std::shared_ptr<Texture> TextureAssetImporter::ProcessRawData(const unsigned char* data, const int size, int& width, int& height, int& channels, const int desiredChannels) {
		if(!data) {
			DEBUG_ERROR("Invalid data provided!");
			return nullptr;
		}

		unsigned char* textureData = stbi_load_from_memory(data, size, &width, &height, &channels, desiredChannels);
		if(!textureData) {
			DEBUG_ERROR("Failed to load texture data!");
			return nullptr;
		}

		std::shared_ptr<Texture> loadedTexture = std::shared_ptr<Texture>(new Texture());
		loadedTexture->textureData = std::unique_ptr<unsigned char>(textureData);
		return loadedTexture;
	}
}
