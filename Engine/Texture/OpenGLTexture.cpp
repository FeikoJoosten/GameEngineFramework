#include "Engine/Texture/OpenGLTexture.hpp"

#ifdef USING_OPENGL
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"

#include <GL/glew.h>
#include <stb/stb_image.h>

namespace Engine {
	OpenGLTexture::OpenGLTexture(const std::string& filename, const int desiredChannels) : Texture(filename, desiredChannels) {
		const std::string baseLocation = "Resources/Textures/" + filename;
		const std::string defaultTextureLocation = AssetManager::Get()->GetProjectRoot() + "Resources/Textures/default.png";
		stbi_uc* textureData = stbi_load(
			AssetManager::FileExists(baseLocation) ?
			(AssetManager::Get()->GetProjectRoot() + baseLocation).c_str() :
			defaultTextureLocation.c_str(),
			&width, &height, &channels, STBI_rgb_alpha);
		OpenGLTexture::CreateTextureWithData(textureData, true);
		stbi_image_free(textureData);
	}

	OpenGLTexture::OpenGLTexture(int width, int height) : Texture(width, height) {}

	OpenGLTexture::~OpenGLTexture() {
		GLuint textureReference = GLuint(texture);

		if (texture)
			glDeleteTextures(1, &textureReference);
	}

	void OpenGLTexture::CreateTextureWithData(stbi_uc* data, bool genMipMaps, TextureDataSize bytes, bool storage) {
		this->dataSize = bytes;
		GLuint textureReference = GLuint(texture);
		if (textureReference)
			glDeleteTextures(1, &textureReference);

		glGenTextures(1, &textureReference);									// Gen    
		glCheckError();

		glBindTexture(GL_TEXTURE_2D, textureReference);                          // Bind
		glCheckError();

		//
		if (genMipMaps)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    // Minmization
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                   // Minmization
		glCheckError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);                       // Magnification
		glCheckError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glCheckError();

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
		glCheckError();

		if (genMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		glCheckError();

		texture = uint64_t(textureReference);
	}
} //namespace Engine
#endif