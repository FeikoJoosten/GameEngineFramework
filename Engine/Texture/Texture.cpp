#include "Engine/Texture/Texture.hpp"

CEREAL_REGISTER_TYPE(Engine::Texture);

namespace Engine
{
	int Texture::GetWidth() const {
		return width;
	}

	int Texture::GetHeight() const {
		return height;
	}

	int Texture::GetNumberOfChannels() const {
		return channels;
	}

	const unsigned char* Texture::GetRawTextureData() const {
		return textureData.data();
	}
}
