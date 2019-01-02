#include "Engine/Texture/Texture.hpp"

namespace Engine
{
	Texture::Texture(const eastl::string& filename, int desiredChannels) : fileName(filename)
	{
	}

	Texture::Texture(int width, int height) : texture(0), width(0), height(0) {}

	Texture::~Texture() {}

	const uint64_t Texture::GetTexture()
	{
		return texture;
	}

	const eastl::string Texture::GetFileName()
	{
		return fileName;
	}

	void Texture::CreateTextureWithData(stbi_uc* data, bool genMipMaps) {}

} //namespace Engine