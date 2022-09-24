#pragma once

#include "Engine/Api/Api.hpp"
#include <string>
#include <glm/glm.hpp>
#include <stb/stb_image.h>

namespace Engine
{
	enum class ENGINE_API TextureDataSize {
		U_CHAR = 0,
		S_CHAR,
		U_SHORT,
		S_SHORT,
		U_INT,
		S_INT
	};

	/// <summary>
	/// This object is used to store information regarding a texture. NOTE: Only the resource manager is allowed to create this object.
	/// </summary>
	class ENGINE_API Texture
	{
	private:
		friend class ResourceManager;
		friend class Skeleton;
#ifdef USING_OPENGL
		friend class OpenGLTexture;
#endif
#ifdef USING_VULKAN
		friend class VulkanTexture;
#endif
		/// <summary>
		/// This constructor allows you to create a texture with the given size. You will have to provide your own data.
		/// </summary>
		/// <param name="width">The width of the texture you want to create.</param>
		/// <param name="height">The height of the texture you want to create.</param>
		Texture(int width, int height);

		/// <summary>
		/// Destructor of texture object.
		/// </summary>
	public:
		virtual ~Texture();
	public:
		/// Get the texture ID in OpenGL
		/// <summary>
		/// This method allows you to get the texture number assigned by the graphics API.
		/// </summary>
		/// <returns>Returns the texture number as un uint64_t.</returns>
		uint64_t GetTexture() const;

		/// <summary>
		/// This method allows you to retrieve the name of this texture.
		/// </summary>
		/// <returns>Returns the name of this texture as a std::string.</returns>
		std::string GetFileName()const;

		/// <summary>
		/// This method allows you to create a texture from RGBA provided data.
		/// </summary>
		/// <param name="data">The data of this texture.</param>
		/// <param name="genMipMaps">This bool defines if this texture should generate mip maps.</param>
		/// <param name="bytes">The number of bytes per color. Default is 1 (single char)</param>
		/// <param name="storage">Used by Vulkan only. Defines if you want to use VK_IMAGE_USAGE_STORAGE_BIT or VK_IMAGE_USAGE_SAMPLED_BIT.</param>
		virtual void CreateTextureWithData(stbi_uc* data, bool genMipMaps, TextureDataSize bytes = TextureDataSize::U_CHAR, bool storage = false);

		/// <summary>
		/// The color of this texture.
		/// </summary>
		glm::vec4 mainColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		/// temp for vulkan
		//unsigned char* pixelData;
		/// <summary>
		/// This method allows you to get the width of this texture.
		/// </summary>
		/// <returns>Returns the width of this texture as an int.</returns>
		int getWidth() const { return width; }
		/// <summary>
		/// This method allows you to get the height of this texture.
		/// </summary>
		/// <returns>Returns the height of this texture as an int.</returns>
		int getHeight() const { return height; }

		/// <summary>
		/// This method allows you to compare a texture against another texture.
		/// </summary>
		/// <param name="texture">The texture you want to compare against.</param>
		/// <returns>Returns true if both textures are equal to each other otherwise it returns false.</returns>
		bool operator==(const Texture& texture);
		/// <summary>
		/// This method allows you to compare a texture against another texture.
		/// </summary>
		/// <param name="texture">The texture you want to compare against.</param>
		/// <returns>Returns true if the textures are not equal to each other otherwise it returns false.</returns>
		bool operator!=(const Texture& texture);
	protected:

		std::string fileName;
		TextureDataSize dataSize;
		uint64_t texture = 0;
		int width = 0;
		int height = 0;
		int channels = 0;

		/// <summary>
		/// Creates a texture by loading the data from a texture file. This method used stb_image to load the image.
		/// </summary>
		/// <param name="filename">This is the name of the texture you want to load. NOTE: The texture needs to be in the folder 'Resources/Textures/' (It can be in a subfolder, as long as it's in the Textures folder). And the extension type needs to be added as well.</param>
		/// <param name="desiredChannels">The amount of desired channels for this texture. This value is 4 by default.</param>
		explicit Texture(const std::string& filename, int desiredChannels = 4);
	};
} //namespace Engine