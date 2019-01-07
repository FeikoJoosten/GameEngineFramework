#include "Engine/Renderer/Vulkan/VulkanDescriptorPool.hpp"
#ifdef USING_VULKAN

#include <iostream>

#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/string.h>

namespace Engine {

	VulkanDescriptorPool::VulkanDescriptorPool(VulkanLogicalDevice* device)
	{
		this->device = device;
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		if (pool != 0)
			vkDestroyDescriptorPool(device->GetDevice(), pool, nullptr);
	}

	void VulkanDescriptorPool::AddPoolSize(VkDescriptorType type, uint32_t count)
	{
		VkDescriptorPoolSize size = {};
		size.type = type;
		size.descriptorCount = count;

		poolSizes.push_back(size);
	}

	void VulkanDescriptorPool::Compile(uint32_t maxSets)
	{
		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		createInfo.pPoolSizes = poolSizes.data();
		createInfo.maxSets = maxSets;
		createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		VkResult res = vkCreateDescriptorPool(device->GetDevice(), &createInfo, nullptr, &pool);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create descriptor pool";
			std::cout << s.c_str() << std::endl;
		}
	}

	VkDescriptorPool VulkanDescriptorPool::GetPool() const
	{
		return pool;
	}

	void VulkanDescriptorPool::AllocateDescriptorSet(uint32_t descriptorSetCount, VkDescriptorSetLayout * layouts, VkDescriptorSet * sets) const
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = descriptorSetCount;
		allocInfo.pSetLayouts = layouts;

		VkResult res = vkAllocateDescriptorSets(device->GetDevice(), &allocInfo, sets);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to allocate descriptor sets";
			std::cout << s.c_str() << std::endl;
		}
	}

	void VulkanDescriptorPool::DescriptorSetBindToBuffer(VkDescriptorSet set, VkBuffer buffer, VkDeviceSize offset,
		VkDeviceSize range, uint32_t binding, uint32_t arrayElement,
		VkDescriptorType type, uint32_t descriptorCount) const
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer;
		bufferInfo.offset = offset;
		bufferInfo.range = range;
		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = set;
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = arrayElement;
		descriptorWrite.descriptorType = type;
		descriptorWrite.descriptorCount = descriptorCount;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device->GetDevice(), 1, &descriptorWrite, 0, nullptr);
	}

	void VulkanDescriptorPool::DescriptorSetBindToImage(VkDescriptorSet set, VkImageLayout layout, VkImageView imageView, VkSampler sampler, uint32_t binding, uint32_t arrayElement, VkDescriptorType type, uint32_t descriptorCount) const
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = layout;
		imageInfo.imageView = imageView;
		imageInfo.sampler = sampler;

		VkWriteDescriptorSet writeInfo = {};
		writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet = set;
		writeInfo.dstBinding = binding;
		writeInfo.dstArrayElement = arrayElement;
		writeInfo.descriptorType = type;
		writeInfo.descriptorCount = descriptorCount;
		writeInfo.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device->GetDevice(), 1, &writeInfo, 0, nullptr);
	}

}

#endif // USING_VULKAN