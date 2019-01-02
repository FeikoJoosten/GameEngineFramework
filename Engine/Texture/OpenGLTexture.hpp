#pragma once

#include "Engine/Texture/Texture.hpp"

namespace Engine
{
	class ENGINE_API OpenGLTexture : public Texture
	{
	public:
		~OpenGLTexture() override;
		void CreateTextureWithData(stbi_uc* data, bool genMipMaps) override;
	private:
		friend class ModelLoader;
		OpenGLTexture(const eastl::string& filename, int desiredChannels = 4);
	};
}
