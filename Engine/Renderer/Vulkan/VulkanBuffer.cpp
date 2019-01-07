#include "Engine/Renderer/Vulkan/VulkanBuffer.hpp"
#ifdef USING_VULKAN

namespace Engine {

	VulkanBuffer::VulkanBuffer(VulkanLogicalDevice * device, VmaAllocator allocator, uint32_t size, VkBufferUsageFlags usage, bool gpu, VkCommandPool pool)
	{
		this->device = device;
		this->allocator = allocator;
		this->size = size;
		this->usage = usage;
		this->gpu = gpu;
		this->pool = pool;

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocInfo = {};
		if (gpu)
			allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		else
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		vmaCreateBuffer(allocator, &createInfo, &allocInfo, &buffer, &allocation, &allocationInfo);
		
	}

	VulkanBuffer::VulkanBuffer(VulkanLogicalDevice * device, VmaAllocator allocator, uint32_t size, VkBufferUsageFlags usage, VmaMemoryUsage memoryType, VkCommandPool pool)
	{
		this->device = device;
		this->allocator = allocator;
		this->size = size;
		this->usage = usage;
		this->gpu = gpu;
		this->pool = pool;

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = memoryType;

		if (memoryType == VMA_MEMORY_USAGE_GPU_ONLY)
			gpu = true;
		else
			gpu = false;
		
		vmaCreateBuffer(allocator, &createInfo, &allocInfo, &buffer, &allocation, &allocationInfo);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		vkQueueWaitIdle(device->GetGraphicsQueue());
		vmaDestroyBuffer(allocator, buffer, allocation);
	}

	void VulkanBuffer::CreateBufferView(uint32_t range, uint32_t offset, VkFormat format)
	{
		VkBufferViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
		viewInfo.buffer = buffer;
		viewInfo.range = range;
		viewInfo.offset = offset;
		viewInfo.format = format;
		viewInfo.flags = 0;

		vkCreateBufferView(device->GetDevice(), &viewInfo, nullptr, &view);

	}

	VkBuffer VulkanBuffer::GetBuffer() const
	{
		return buffer;
	}

	VkBufferView VulkanBuffer::GetBufferView() const
	{
		return view;
	}

	VmaAllocationInfo VulkanBuffer::getAllocationInfo()
	{
		return allocationInfo;
	}

	void VulkanBuffer::UpdateBuffer(void * data, uint32_t offset, uint32_t size)
	{
		if (gpu) {
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.size = size;
			createInfo.usage = (usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT) & (~VK_BUFFER_USAGE_TRANSFER_DST_BIT);

			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

			VkBuffer stagingBuffer;
			VmaAllocation stagingAllocation;

			vmaCreateBuffer(allocator, &createInfo, &allocInfo, &stagingBuffer, &stagingAllocation, nullptr);

			void* dst;
			vmaMapMemory(allocator, stagingAllocation, &dst);
			memcpy((static_cast<char*>(dst) + offset), data, size);
			vmaUnmapMemory(allocator, stagingAllocation);

			VkCommandBufferAllocateInfo cmdAllocInfo = {};
			cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdAllocInfo.commandBufferCount = 1;
			cmdAllocInfo.commandPool = pool;
			cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

			VkCommandBuffer cmdBuffer;

			vkAllocateCommandBuffers(device->GetDevice(), &cmdAllocInfo, &cmdBuffer);

			VkCommandBufferBeginInfo cmdBeginInfo = {};
			cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);

			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = offset;
			copyRegion.dstOffset = offset;
			copyRegion.size = size;
			vkCmdCopyBuffer(cmdBuffer, stagingBuffer, buffer, 1, &copyRegion);

			vkEndCommandBuffer(cmdBuffer);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &cmdBuffer;

			vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(device->GetGraphicsQueue());

			vkFreeCommandBuffers(device->GetDevice(), pool, 1, &cmdBuffer);

			vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);

		}
		else {
			void* dst;
			vmaMapMemory(allocator, allocation, &dst);
			memcpy((static_cast<char*>(dst) + offset), data, size);
			vmaUnmapMemory(allocator, allocation);
		}
	}

	void VulkanBuffer::MapBuffer(void * location)
	{
		vmaMapMemory(allocator, allocation, &location);
	}

	void VulkanBuffer::UnmapBuffer()
	{
		vmaUnmapMemory(allocator, allocation);
	}

	void VulkanBuffer::ClearBuffer() const
	{
		vkQueueWaitIdle(device->GetGraphicsQueue());

		VkCommandBufferAllocateInfo cmdAllocInfo = {};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.commandBufferCount = 1;
		cmdAllocInfo.commandPool = pool;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkCommandBuffer cmdBuffer;

		vkAllocateCommandBuffers(device->GetDevice(), &cmdAllocInfo, &cmdBuffer);

		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);

		vkCmdFillBuffer(cmdBuffer, buffer, 0, VK_WHOLE_SIZE, 0);

		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device->GetGraphicsQueue());

		vkFreeCommandBuffers(device->GetDevice(), pool, 1, &cmdBuffer);
	}
}

#endif // USING_VULKAN