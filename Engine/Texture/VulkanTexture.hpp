#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include "Engine/Texture/Texture.hpp"
#include "Engine/Renderer/Vulkan/vk_mem_alloc.h"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Renderer/Vulkan/VulkanDescriptorPool.hpp"

#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	class ResourceManager;
	class VulkanRenderer;

	class ENGINE_API VulkanTexture : public Texture
	{
		friend class ResourceManager;
		friend class VulkanRenderer;

		static void InitTextureSystem(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool, VmaAllocator allocator, VkCommandPool commandPool);

	public:
		~VulkanTexture() override;
		void CreateTextureWithData(stbi_uc* data, bool genMipMaps, TextureDataSize bytes = TextureDataSize::U_CHAR, bool storage = false) override;

		void SetSampler(VkSamplerCreateInfo samplerInfo);

		VulkanTexture(int width, int height);

		VkImage GetImage() const;
		VkImageView GetImageView() const;
		VkSampler GetSampler() const;

		VkDescriptorSet CreateDescriptorSet(size_t threadID, size_t pipelineID, size_t set,
			VkDescriptorSetLayout layout);

		VkDescriptorSet GetDescriptorSet(size_t threadID, size_t pipelineID, size_t set);

	private:
		friend class VulkanRenderer;

		VulkanTexture(const eastl::string& filename, int desiredChannels = 4);

		VkImage image;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;

		VkImageView imageView;

		VkSampler sampler;

		bool storage;

		eastl::vector<eastl::vector<eastl::vector<VkDescriptorSet>>> descriptorSets;

		static VulkanRenderer* renderer;
		static VulkanLogicalDevice* device;
		static VulkanDescriptorPool* descriptorPool;
		static VmaAllocator allocator;
		static VkCommandPool commandPool;

	};
} //namespace Engine
#endif