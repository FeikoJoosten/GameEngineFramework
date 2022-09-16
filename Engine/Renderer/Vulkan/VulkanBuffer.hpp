#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN

#include <vulkan/vulkan.h>

#include <Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp>
#include <Engine/Renderer/Vulkan/vk_mem_alloc.h>

namespace Engine {

	class VulkanBuffer
	{
	public:
		VulkanBuffer(VulkanLogicalDevice* device, VmaAllocator allocator, uint32_t size, VkBufferUsageFlags usage, bool gpu, VkCommandPool pool);
		VulkanBuffer(VulkanLogicalDevice* device, VmaAllocator allocator, uint32_t size, VkBufferUsageFlags usage, VmaMemoryUsage memoryType, VkCommandPool pool);
		~VulkanBuffer();

		void CreateBufferView(uint32_t range, uint32_t offset, VkFormat format);

		VkBuffer GetBuffer() const;

		VkBufferView GetBufferView() const;

		VmaAllocationInfo getAllocationInfo();

		void UpdateBuffer(void* data, uint32_t offset, uint32_t size);

		void MapBuffer(void* location);

		void UnmapBuffer();

		void ClearBuffer() const;

	protected:
		VmaAllocator allocator;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;

		VkBuffer buffer;

		VkBufferView view;

		VkBufferUsageFlags usage;

		uint32_t size;

		VulkanLogicalDevice* device;

		VkCommandPool pool;

		bool gpu;


	};

}

#endif // USING_VULKAN