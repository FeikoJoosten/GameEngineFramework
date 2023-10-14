#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Texture/Texture.hpp"

namespace Engine
{
	class ENGINE_API OpenGLTexture : public Texture
	{
	public:
		virtual ~OpenGLTexture() override;
		OpenGLTexture(const OpenGLTexture& other) = delete;
		OpenGLTexture(OpenGLTexture&& other) noexcept = delete;

		OpenGLTexture& operator=(const OpenGLTexture& other) = delete;
		OpenGLTexture& operator=(OpenGLTexture&& other) noexcept = delete;

	private:
		unsigned int textureId;

		friend class ResourceManager;
		friend class ShaderParameter;
		//friend class TextureImporter;

		/// <summary>
		/// This method allows you to create a texture from RGBA provided data.
		/// </summary>
		/// <param name="data">The data of this texture.</param>
		/// <param name="genMipMaps">This bool defines if this texture should generate mip maps.</param>
		static Texture* CreateTextureWithData(unsigned char* data, bool genMipMaps, TextureDataSize bytes = TextureDataSize::UChar);

		/// <summary>
		/// Creates a texture by loading the data from a texture file. This method used stb_image to load the image.
		/// </summary>
		/// <param name="filename">This is the name of the texture you want to load. NOTE: The texture needs to be in the folder 'Resources/Textures/' (It can be in a subfolder, as long as it's in the Textures folder). And the extension type needs to be added as well.</param>
		/// <param name="desiredChannels">The amount of desired channels for this texture. This value is 4 by default.</param>
		explicit OpenGLTexture(const std::string& filename, int desiredChannels = 4);

		/// Get the texture ID in OpenGL
		/// <summary>
		/// This method allows you to get the texture id assigned by OpenGL.
		/// </summary>
		/// <returns>Returns the texture number as an unsigned int.</returns>
		[[nodiscard]] unsigned int GetTexture() const;
	};
} //namespace Engine
#endif