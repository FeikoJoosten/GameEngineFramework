#include "Engine/Texture/Texture.hpp"

namespace Engine
{
	Texture::Texture(const std::string& filename, int desiredChannels) : fileName(filename), dataSize()
	{
	}

	Texture::Texture(int width, int height) : dataSize(), texture(0), width(width), height(height)
	{
	}

	Texture::~Texture()
	{
	}

	uint64_t Texture::GetTexture() const
	{
		return texture;
	}

	std::string Texture::GetFileName() const
	{
		return fileName;
	}

	void Texture::CreateTextureWithData(stbi_uc* data, bool genMipMaps, TextureDataSize bytes, bool storage)
	{
	}

	bool Texture::operator==(const Texture& texture)
	{
		if (mainColor != texture.mainColor) return false;
		if (width != texture.width) return false;
		if (height != texture.height) return false;
		if (channels != texture.channels) return false;
		if (this->texture != texture.texture) return false;
		if (fileName != texture.fileName) return false;

		return false;
	}

	bool Texture::operator!=(const Texture& texture)
	{
		if (mainColor != texture.mainColor) return true;
		if (width != texture.width) return true;
		if (height != texture.height) return true;
		if (channels != texture.channels) return true;
		if (this->texture != texture.texture) return true;
		if (fileName != texture.fileName) return true;

		return false;
	}
} //namespace Engine