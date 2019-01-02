#include "Engine/Texture/VulkanTexture.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"
#include "Engine/engine.hpp"
#include "Engine/Utility/Utility.hpp"

namespace Engine
{

	VulkanTexture::VulkanTexture(const eastl::string& filename, int desiredChannels) : Texture(filename, desiredChannels)
	{
		eastl::string baseLocation = "Resources/Textures/";
		baseLocation.append(filename);

		stbi_uc* textureData = stbi_load(
			Utility::FileExists(baseLocation.c_str()) ? 
			baseLocation.c_str() : 
			"Resources/Textures/default.png", 
			&width, &height, &channels, STBI_rgb_alpha);

		VulkanTexture::CreateTextureWithData(textureData, true); 
		
		stbi_image_free(textureData);
	}

	VulkanTexture::~VulkanTexture()
	{

	}

	void VulkanTexture::CreateTextureWithData(stbi_uc* data, bool genMipMaps)
	{
		VulkanRenderer* vulkanRenderer = Engine::GetRenderer<VulkanRenderer>();

		vulkanRenderer->CreateTextureImage(getWidth(), getHeight(), data, &textureImage, &textureImageMemory);
		vulkanRenderer->CreateTextureImageView(&textureImageView, &textureImage);
		vulkanRenderer->CreateTextureSampler(&textureSampler, false);
		
	}

	void VulkanTexture::CreateDescriptorSet(VkBuffer* uniformBuffer)
	{
		VulkanRenderer* vulkanRenderer = Engine::GetRenderer<VulkanRenderer>();
		vulkanRenderer->CreateDescriptorSet(&descriptorSet, uniformBuffer, &textureImageView, &textureSampler);
	}
}
