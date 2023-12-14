#pragma once

#include "Engine/Utility/Defines.hpp"

#ifdef USING_OPENGL
#include "Engine/Texture/Texture.hpp"

namespace Engine
{
	class ENGINE_LOCAL OpenGLTexture
	{
		unsigned int textureId;
		std::shared_ptr<Texture> relatedTexture;

	public:
		OpenGLTexture() = delete;
		explicit OpenGLTexture(const std::shared_ptr<Texture>& relatedTexture);
		~OpenGLTexture();
		OpenGLTexture(const OpenGLTexture& other) = delete;
		OpenGLTexture(OpenGLTexture&& other) noexcept = delete;

		OpenGLTexture& operator=(const OpenGLTexture& other) = delete;
		OpenGLTexture& operator=(OpenGLTexture&& other) noexcept = delete;

		[[nodiscard]] unsigned int GetTextureId() const;
	};
}
#endif