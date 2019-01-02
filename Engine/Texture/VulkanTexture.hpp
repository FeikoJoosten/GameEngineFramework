#pragma once

#include "Engine/Texture/Texture.hpp"
#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>

namespace Engine
{
	class ENGINE_API VulkanTexture : public Texture
	{
	public:
		~VulkanTexture() override;
		void CreateTextureWithData(stbi_uc* data, bool genMipMaps) override;
		void CreateDescriptorSet(VkBuffer* uniformBuffer);
		VkImage textureImage = VK_NULL_HANDLE;
		VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
		VkImageView textureImageView = VK_NULL_HANDLE;
		VkSampler textureSampler = VK_NULL_HANDLE;

		VkDescriptorSet* getDescriptorSet() { return &descriptorSet; }
	private:
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

		friend class ModelLoader;
		VulkanTexture(const eastl::string& filename, int desiredChannels = 4);
	};
}
