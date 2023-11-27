#pragma once

#include "Engine/AssetManagement/AssetImportSettings.hpp"
#include "Engine/Texture/TextureDataSize.hpp"

namespace Engine {
    class TextureAssetImportSettings : public AssetImportSettings {
		friend cereal::access;
    public:
        bool generateMipMaps = true;
		int mipMapLevel = 0;
		TextureDataSize dataSize {};

		explicit TextureAssetImportSettings() = default;
		virtual ~TextureAssetImportSettings() override = default;
		TextureAssetImportSettings(const TextureAssetImportSettings& other) = delete;
		TextureAssetImportSettings(TextureAssetImportSettings&& other) = delete;
		TextureAssetImportSettings& operator=(const TextureAssetImportSettings& other) = delete;
		TextureAssetImportSettings& operator=(TextureAssetImportSettings&& other) = delete;
	private:

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void TextureAssetImportSettings::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<AssetImportSettings>(this)),
			CEREAL_NVP(generateMipMaps),
			CEREAL_NVP(dataSize)
		);
	}
}
