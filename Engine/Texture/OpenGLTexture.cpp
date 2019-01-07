#include "Engine/Texture/OpenGLTexture.hpp"
#ifdef USING_OPENGL
#include "Engine/Utility/Utility.hpp"

#include <ThirdParty/glew-2.1.0/include/GL/glew.h>

namespace Engine
{
	OpenGLTexture::OpenGLTexture(const eastl::string& filename, int desiredChannels) : Texture(filename, desiredChannels)
	{
		eastl::string baseLocation = "Resources/Textures/";
		baseLocation.append(filename);
		stbi_uc* textureData = stbi_load(
			Utility::FileExists(baseLocation) ?
			baseLocation.c_str() :
			"Resources/Textures/default.png", &width, &height, &channels, STBI_rgb_alpha);
		OpenGLTexture::CreateTextureWithData(textureData, true);
		stbi_image_free(textureData);
	}

	OpenGLTexture::OpenGLTexture(int width, int height) : Texture(width, height)
	{
	}

	OpenGLTexture::~OpenGLTexture()
	{
		GLuint textureReference = GLuint(texture);

		if (texture)
			glDeleteTextures(1, &textureReference);
	}

	void OpenGLTexture::CreateTextureWithData(stbi_uc* data, bool genMipMaps, TextureDataSize bytes, bool storage)
	{
		this->dataSize = bytes;
		GLuint textureReference = GLuint(texture);
		if (textureReference)
			glDeleteTextures(1, &textureReference);

		glGenTextures(1, &textureReference);									// Gen    
		glGetError();

		glBindTexture(GL_TEXTURE_2D, textureReference);                          // Bind
		glGetError();

		//
		if (genMipMaps)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    // Minmization
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                   // Minmization
		glGetError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);                       // Magnification
		glGetError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glGetError();

		GLenum type;

		switch (bytes) {
		case TextureDataSize::U_CHAR:
			type = GL_UNSIGNED_BYTE;
			break;
		case TextureDataSize::S_CHAR:
			type = GL_UNSIGNED_BYTE;
			break;
		case TextureDataSize::U_SHORT:
			type = GL_UNSIGNED_SHORT;
			break;
		case TextureDataSize::S_SHORT:
			type = GL_UNSIGNED_SHORT;
			break;
		case TextureDataSize::U_INT:
			type = GL_UNSIGNED_INT;
			break;
		case TextureDataSize::S_INT:
			type = GL_UNSIGNED_INT;
			break;
		default:
			type = GL_UNSIGNED_BYTE;
			break;
		}

		glTexImage2D(
			GL_TEXTURE_2D,						// What (target)
			0,									// Mip-map level
			GL_RGBA,		                    // Internal format
			width,								// Width
			height,								// Height
			0,									// Border
			GL_RGBA,							// Format (how to use)
			type,					// Type   (how to intepret)
			data);								// Data
		glGetError();

		if (genMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		glGetError();

		texture = uint64_t(textureReference);
	}
} //namespace Engine
#endif