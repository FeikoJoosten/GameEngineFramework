#pragma once

#include "Engine/api.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/string.h>
#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/stb/stb_image.h>

namespace Engine
{
	class ENGINE_API Texture
	{
	public:

		/// Creates an empty texture with given size. You will need to provide
		/// the data.
		Texture(int width, int height);

		/// Releases OpenGL resources
		virtual ~Texture();

		/// Get the texture ID in OpenGL
		const uint64_t GetTexture();

		const eastl::string GetFileName();

		/// Creates a texture from RGBA provided data
		virtual void CreateTextureWithData(stbi_uc* data, bool genMipMaps);

		glm::vec4 mainColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		/// temp for vulkan
		unsigned char* pixelData;
		int getWidth() { return width; }
		int getHeight() { return height; }
	protected:

		eastl::string fileName;
		uint64_t texture = 0;
		int width = 0;
		int height = 0;
		int channels = 0;

		friend class ModelLoader;
		/// Create a texture by loading from a file
		/// Uses stb_image (as SOIL does). Check for supported formats
		/// in the stb_image header
		Texture(const eastl::string& filename, int desiredChannels = 4);
	};
} //namespace Engine