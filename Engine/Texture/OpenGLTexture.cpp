#include "Engine/Texture/OpenGLTexture.hpp"

#ifdef USING_OPENGL
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"

#include <GL/glew.h>
#include <stb/stb_image.h>

namespace Engine {
	OpenGLTexture::OpenGLTexture(const std::string& filename, const int desiredChannels) {
		const std::string baseLocation = "Textures/";
		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		const std::string projectRoot = assetManager->GetProjectRoot();
		const std::string defaultTextureLocation = projectRoot + "Resources/Textures/default.png";
		stbi_uc* textureData = stbi_load(
			assetManager->FileExists(baseLocation, baseLocation + filename, true) ?
			(projectRoot + "Resources/" + baseLocation + filename).c_str() :
			defaultTextureLocation.c_str(),
			&width, &height, &channels, STBI_rgb_alpha);
		OpenGLTexture::CreateTextureWithData(textureData, true);
		stbi_image_free(textureData);
	}

	unsigned int OpenGLTexture::GetTexture() const {
		return textureId;
	}

	OpenGLTexture::~OpenGLTexture() {
		GLuint textureReference = GLuint(textureId);

		if (textureId)
			glDeleteTextures(1, &textureReference);
	}

	Texture* OpenGLTexture::CreateTextureWithData(stbi_uc* data, bool genMipMaps, TextureDataSize bytes) {
		return nullptr;
		//this->dataSize = bytes;
		//GLuint textureReference = GLuint(textureId);
		//if (textureReference)
		//	glDeleteTextures(1, &textureReference);
		//
		//glGenTextures(1, &textureReference);									// Gen    
		//glCheckError();
		//
		//glBindTexture(GL_TEXTURE_2D, textureReference);                          // Bind
		//glCheckError();
		//
		////
		//if (genMipMaps)
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    // Minmization
		//else
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                   // Minmization
		//glCheckError();
		//
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);                       // Magnification
		//glCheckError();
		//
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glCheckError();
		//
		//GLenum type;
		//
		//switch (bytes) {
		//case TextureDataSize::UChar:
		//	type = GL_UNSIGNED_BYTE;
		//	break;
		//case TextureDataSize::SChar:
		//	type = GL_UNSIGNED_BYTE;
		//	break;
		//case TextureDataSize::UShort:
		//	type = GL_UNSIGNED_SHORT;
		//	break;
		//case TextureDataSize::SShort:
		//	type = GL_UNSIGNED_SHORT;
		//	break;
		//case TextureDataSize::UInt:
		//	type = GL_UNSIGNED_INT;
		//	break;
		//case TextureDataSize::SInt:
		//	type = GL_UNSIGNED_INT;
		//	break;
		//default:
		//	type = GL_UNSIGNED_BYTE;
		//	break;
		//}
		//
		//glTexImage2D(
		//	GL_TEXTURE_2D,						// What (target)
		//	0,									// Mip-map level
		//	GL_RGBA,		                    // Internal format
		//	width,								// Width
		//	height,								// Height
		//	0,									// Border
		//	GL_RGBA,							// Format (how to use)
		//	type,								// Type   (how to intepret)
		//	data);								// Data
		//glCheckError();
		//
		//if (genMipMaps)
		//	glGenerateMipmap(GL_TEXTURE_2D);
		//
		//glCheckError();
		//
		//textureId = uint64_t(textureReference);
	}
} //namespace Engine
#endif