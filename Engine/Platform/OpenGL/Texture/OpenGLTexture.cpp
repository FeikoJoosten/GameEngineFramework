#include "Engine/Platform/OpenGL/Texture/OpenGLTexture.hpp"
#include "Engine/Utility/Defines.hpp"

#ifdef USING_OPENGL
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/Platform/OpenGL/Renderer/OpenGLUtility.hpp"
#include "Engine/Texture/TextureAssetImportSettings.hpp"
#include "Engine/Utility/Logging.hpp"

#include <GL/glew.h>

namespace Engine {
	OpenGLTexture::OpenGLTexture(const std::shared_ptr<Texture>& relatedTexture) {
		this->relatedTexture = relatedTexture;

		std::shared_ptr<AssetImportSettings> importSettings;
		AssetRegistry::TryGetImportSettingsForAsset(relatedTexture->GetGuid(), importSettings);
		const std::shared_ptr<TextureAssetImportSettings> textureAssetImportSettings = std::static_pointer_cast<TextureAssetImportSettings>(importSettings);

		GLenum format;
		switch (relatedTexture->GetNumberOfChannels()) {
		case 1:
			format = GL_R;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			DEBUG_ERROR("Not supported number of channels! Incoming number of channels: " + relatedTexture->GetNumberOfChannels());
			return;
		}

		GLuint textureReference;
		glGenTextures(1, &textureReference);
		glCheckError();

		glBindTexture(GL_TEXTURE_2D, textureReference);
		glCheckError();

		if (textureAssetImportSettings->generateMipMaps)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glCheckError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glCheckError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glCheckError();

		GLenum type;
		switch (textureAssetImportSettings->dataSize) {
		case TextureDataSize::Char:
			type = GL_UNSIGNED_BYTE;
			break;
		case TextureDataSize::Short:
			type = GL_UNSIGNED_SHORT;
			break;
		case TextureDataSize::Int:
			type = GL_UNSIGNED_INT;
			break;
		default:
			type = GL_UNSIGNED_BYTE;
			break;
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			textureAssetImportSettings->mipMapLevel,
			format,
			relatedTexture->GetWidth(),
			relatedTexture->GetHeight(),
			0,
			format,
			type,
			relatedTexture->GetRawTextureData());
		glCheckError();

		if (textureAssetImportSettings->generateMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		glCheckError();

		textureId = textureReference;
	}

	OpenGLTexture::~OpenGLTexture() {
		if (textureId)
			glDeleteTextures(1, &textureId);
	}

	unsigned int OpenGLTexture::GetTextureId() const {
		return textureId;
	}
}
#endif