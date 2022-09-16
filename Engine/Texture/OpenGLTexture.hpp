#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Texture/Texture.hpp"

namespace Engine
{
	/// <summary>
	/// This object is used to store information regarding an OpenGLTexture. NOTE: Only the resource manager is allowed to create this object.
	/// </summary>
	class ENGINE_API OpenGLTexture : public Texture
	{
	public:
		/// <summary>
		/// This deconstructor unloads the texture from the memory.
		/// </summary>
		~OpenGLTexture() override;
		/// <summary>
		/// This method allows you to create a texture from RGBA provided data.
		/// </summary>
		/// <param name="data">The data of this texture.</param>
		/// <param name="genMipMaps">This bool defines if this texture should generate mip maps.</param>
		void CreateTextureWithData(stbi_uc* data, bool genMipMaps, TextureDataSize bytes = TextureDataSize::U_CHAR, bool storage = false) override;
	private:
		friend class Skeleton;
		friend class ResourceManager;

		/// <summary>
		/// Creates a texture by loading the data from a texture file. This method used stb_image to load the image.
		/// </summary>
		/// <param name="filename">This is the name of the texture you want to load. NOTE: The texture needs to be in the folder 'Resources/Textures/' (It can be in a subfolder, as long as it's in the Textures folder). And the extension type needs to be added as well.</param>
		/// <param name="desiredChannels">The amount of desired channels for this texture. This value is 4 by default.</param>
		explicit OpenGLTexture(const std::string& filename, int desiredChannels = 4);
		/// <summary>
		/// Creates an empty texture object. You will have to add the data yourself.
		/// </summary>
		/// <param name="width">The width of the texture you want to create.</param>
		/// <param name="height">The height of the texture you want to create.</param>
		explicit OpenGLTexture(int width, int height);
	};
} //namespace Engine
#endif