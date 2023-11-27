#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

namespace Engine {
	class ENGINE_API Texture final : public Asset {
		friend cereal::access;
		friend class TextureAssetImporter;

		int width = 0;
		int height = 0;
		int channels = 0;
		std::vector<unsigned char> textureData;

		explicit Texture() = default;
	public:

		virtual ~Texture() override = default;
		Texture(const Texture& other) = delete;
		Texture(Texture&& other) noexcept = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture& operator=(Texture&& other) noexcept = delete;

		[[nodiscard]] int GetWidth() const;
		[[nodiscard]] int GetHeight() const;
		[[nodiscard]] int GetNumberOfChannels() const;
		[[nodiscard]] const unsigned char* GetRawTextureData() const;

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void Texture::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(width),
			CEREAL_NVP(height),
			CEREAL_NVP(channels)
		);
	}
}
