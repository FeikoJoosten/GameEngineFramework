#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>

namespace Engine {
	enum class ENGINE_API TextureDataSize {
		UChar = 0,
		SChar,
		UShort,
		SShort,
		UInt,
		SInt
	};

	/// <summary>
	/// This object is used to store information regarding a texture. NOTE: Only the resource manager is allowed to create this object.
	/// </summary>
	class ENGINE_API Texture : public Asset {
		friend cereal::access;
		friend class TextureAssetImporter;

	protected:
		int width = 0;
		int height = 0;
		int channels = 0;
		std::unique_ptr<unsigned char> textureData;

		explicit Texture() = default;

	public:

		virtual ~Texture() override = default;
		Texture(const Texture& other) = delete;
		Texture(Texture&& other) noexcept = delete;

		Texture& operator=(const Texture& other) = delete;
		Texture& operator=(Texture&& other) noexcept = delete;

		/// <summary>
		/// This method allows you to get the width of this texture.
		/// </summary>
		/// <returns>Returns the width of this texture as an int.</returns>
		int GetWidth() const;

		/// <summary>
		/// This method allows you to get the height of this texture.
		/// </summary>
		/// <returns>Returns the height of this texture as an int.</returns>
		int GetHeight() const;

		/// <summary>
		/// This method allows you to compare a texture against another texture.
		/// </summary>
		/// <param name="texture">The texture you want to compare against.</param>
		/// <returns>Returns true if both textures are equal to each other otherwise it returns false.</returns>
		bool operator==(const Texture& texture) const;
		/// <summary>
		/// This method allows you to compare a texture against another texture.
		/// </summary>
		/// <param name="texture">The texture you want to compare against.</param>
		/// <returns>Returns true if the textures are not equal to each other otherwise it returns false.</returns>
		bool operator!=(const Texture& texture) const;

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void Texture::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(width),
			CEREAL_NVP(height),
			CEREAL_NVP(channels),
			CEREAL_NVP(textureData)
		);
	}
} //namespace Engine
