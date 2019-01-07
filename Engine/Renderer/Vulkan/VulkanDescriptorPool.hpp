#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN

#include <ThirdParty/Vulkan/Include/vulkan/vulkan.hpp>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine {

	class VulkanLogicalDevice;

	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VulkanLogicalDevice* device);
		~VulkanDescriptorPool();

		void AddPoolSize(VkDescriptorType type, uint32_t count);

		void Compile(uint32_t maxSets);

		VkDescriptorPool GetPool() const;

		void AllocateDescriptorSet(uint32_t descriptorSetCount, VkDescriptorSetLayout* layouts, VkDescriptorSet* sets) const;

		void DescriptorSetBindToBuffer(VkDescriptorSet set, VkBuffer buffer, VkDeviceSize offset,
			VkDeviceSize range, uint32_t binding, uint32_t arrayElement,
			VkDescriptorType type, uint32_t descriptorCount) const;

		void DescriptorSetBindToImage(VkDescriptorSet set, VkImageLayout layout, VkImageView imageView, VkSampler sampler, uint32_t binding, uint32_t arrayElement, VkDescriptorType type, uint32_t descriptorCount) const;

	private:
		eastl::vector<VkDescriptorPoolSize> poolSizes;

		VkDescriptorPool pool;
		VulkanLogicalDevice* device;
	};

}

#endif // USING_VULKAN