#include "Engine/Texture/Texture.hpp"

namespace Engine
{
	int Texture::GetWidth() const {
		return width;
	}

	int Texture::GetHeight() const {
		return height;
	}

	bool Texture::operator==(const Texture& texture) const {
		if (width != texture.width) return false;
		if (height != texture.height) return false;
		if (channels != texture.channels) return false;

		return false;
	}

	bool Texture::operator!=(const Texture& texture) const {
		if (width != texture.width) return true;
		if (height != texture.height) return true;
		if (channels != texture.channels) return true;

		return false;
	}
} //namespace Engine