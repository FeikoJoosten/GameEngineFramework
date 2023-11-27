#include "Engine/Texture/VulkanTexture.hpp"
#ifdef USING_VULKAN
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"
#include "Engine/Engine/Engine.hpp"

namespace Engine {

	VulkanTexture::VulkanTexture(const std::string& filename, int desiredChannels) : Texture(filename, desiredChannels) {
		const std::string baseLocation = "Textures/";
		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		const std::string projectRoot = assetManager->GetProjectRoot();
		const std::string defaultTextureLocation = projectRoot + "Resources/Textures/default.png";
		stbi_uc* textureData = stbi_load(
			assetManager->FileExists(baseLocation, baseLocation + filename, true) ?
			(projectRoot + "Resources/" + baseLocation + filename).c_str() :
			defaultTextureLocation.c_str(),
			&width, &height, &channels, STBI_rgb_alpha);
		VulkanTexture::CreateTextureWithData(textureData, true);
		stbi_image_free(textureData);
	}

	VulkanTexture::VulkanTexture(int width, int height) : Texture(width, height), image(0), imageView(0), sampler(0),
		storage(false) {}

	VulkanRenderer* VulkanTexture::renderer = nullptr;
	VulkanLogicalDevice* VulkanTexture::device = nullptr;
	VulkanDescriptorPool* VulkanTexture::descriptorPool = nullptr;
	VmaAllocator VulkanTexture::allocator = VK_NULL_HANDLE;
	VkCommandPool VulkanTexture::commandPool = VK_NULL_HANDLE;

	void VulkanTexture::InitTextureSystem(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool, VmaAllocator allocator, VkCommandPool commandPool) {
		VulkanTexture::renderer = renderer;
		VulkanTexture::device = device;
		VulkanTexture::descriptorPool = descriptorPool;
		VulkanTexture::allocator = allocator;
		VulkanTexture::commandPool = commandPool;
	}

	VulkanTexture::~VulkanTexture() {
		vkQueueWaitIdle(device->GetGraphicsQueue());

		vkDestroySampler(device->GetDevice(), sampler, nullptr);
		vkDestroyImageView(device->GetDevice(), imageView, nullptr);

		vmaDestroyImage(allocator, image, allocation);
	}

	void VulkanTexture::CreateTextureWithData(stbi_uc* data, bool genMipMaps, TextureDataSize bytes, bool storage) {
		this->dataSize = bytes;
		this->storage = storage;

		uint32_t byteSize;

		switch (bytes) {
		case TextureDataSize::CHAR:
			byteSize = 1;
			break;
		case TextureDataSize::SHORT:
			byteSize = 2;
			break;
		case TextureDataSize::INT:
			byteSize = 4;
			break;
		default:
			byteSize = 1;
			break;
		}

		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		stagingBufferInfo.size = width * height * 4 * byteSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo stagingBufferAllocInfo = {};
		stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferAllocation;

		vmaCreateBuffer(allocator, &stagingBufferInfo, &stagingBufferAllocInfo, &stagingBuffer, &stagingBufferAllocation, nullptr);

		void* dst;

		vmaMapMemory(allocator, stagingBufferAllocation, &dst);
		memcpy(dst, data, static_cast<size_t>(width * height * 4 * byteSize));
		vmaUnmapMemory(allocator, stagingBufferAllocation);

		VkFormat format;

		switch (bytes) {
		case TextureDataSize::U_CHAR:
			format = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case TextureDataSize::S_CHAR:
			format = VK_FORMAT_R8G8B8A8_SNORM;
			break;
		case TextureDataSize::U_SHORT:
			format = VK_FORMAT_R16G16B16A16_UNORM;
			break;
		case TextureDataSize::S_SHORT:
			format = VK_FORMAT_R16G16B16A16_SFLOAT;
			break;
		case TextureDataSize::U_INT:
			format = VK_FORMAT_R32G32B32A32_UINT;
			break;
		case TextureDataSize::S_INT:
			format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		default:
			format = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		}

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = static_cast<uint32_t>(width);
		imageCreateInfo.extent.height = static_cast<uint32_t>(height);
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = format;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (storage)
			imageCreateInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
		else
			imageCreateInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.flags = 0;

		VmaAllocationCreateInfo imageAllocInfo = {};
		imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		vmaCreateImage(allocator, &imageCreateInfo, &imageAllocInfo, &image, &allocation, &allocationInfo);

		VkCommandBuffer commandBuffer;

		VkCommandBufferAllocateInfo cmdAllocInfo = {};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.commandPool = commandPool;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdAllocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(device->GetDevice(), &cmdAllocInfo, &commandBuffer);

		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &cmdBeginInfo);

		VkImageMemoryBarrier barrier1 = {};
		barrier1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier1.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier1.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier1.image = image;
		barrier1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier1.subresourceRange.baseArrayLayer = 0;
		barrier1.subresourceRange.baseMipLevel = 0;
		barrier1.subresourceRange.layerCount = 1;
		barrier1.subresourceRange.levelCount = 1;
		barrier1.srcAccessMask = 0;
		barrier1.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier1);

		VkBufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageSubresource.mipLevel = 0;

		region.imageOffset = { 0,0,0 };
		region.imageExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height),
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VkImageMemoryBarrier barrier2 = {};
		barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		if (storage)
			barrier2.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		else
			barrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier2.image = image;
		barrier2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier2.subresourceRange.baseMipLevel = 0;
		barrier2.subresourceRange.baseArrayLayer = 0;
		barrier2.subresourceRange.layerCount = 1;
		barrier2.subresourceRange.levelCount = 1;
		barrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier2);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		//vkQueueWaitIdle(device->GetGraphicsQueue());

		vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device->GetGraphicsQueue());
		vkFreeCommandBuffers(device->GetDevice(), commandPool, 1, &commandBuffer);

		vmaDestroyBuffer(allocator, stagingBuffer, stagingBufferAllocation);

		VkImageViewCreateInfo viewCreateInfo = {};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = image;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = format;
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;
		viewCreateInfo.subresourceRange.levelCount = 1;

		vkCreateImageView(device->GetDevice(), &viewCreateInfo, nullptr, &imageView);

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 4;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.f;
		samplerInfo.minLod = 0.f;
		samplerInfo.maxLod = 0.f;

		vkCreateSampler(device->GetDevice(), &samplerInfo, nullptr, &sampler);
	}
	void VulkanTexture::SetSampler(VkSamplerCreateInfo samplerInfo) {
		if (sampler != VK_NULL_HANDLE)
			vkDestroySampler(device->GetDevice(), sampler, nullptr);
		vkCreateSampler(device->GetDevice(), &samplerInfo, nullptr, &sampler);

		for (size_t i = 0, size = descriptorSets.size(); i < size; ++i) {
			for (size_t j = 0, size2 = descriptorSets[i].size(); j < size2; ++j) {
				for (size_t k = 0, size3 = descriptorSets[i][j].size(); j < size3; ++k) {
					if (descriptorSets[i][j][k] != VK_NULL_HANDLE) {

						VkDescriptorType type;
						VkImageLayout layout;
						if (storage) {
							type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
							layout = VK_IMAGE_LAYOUT_GENERAL;
						} else {
							type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						}

						renderer->GetDescriptorPool(k)->DescriptorSetBindToImage(descriptorSets[i][j][k],
							layout,
							imageView, sampler, 0, 0,
							type, 1);
					}
				}
			}
		}
	}
	VkImage VulkanTexture::GetImage() const {
		return image;
	}
	VkImageView VulkanTexture::GetImageView() const {
		return imageView;
	}
	VkSampler VulkanTexture::GetSampler() const {
		return sampler;
	}
	VkDescriptorSet VulkanTexture::CreateDescriptorSet(size_t threadID, size_t pipelineID, size_t set, VkDescriptorSetLayout layout) {
		if (descriptorSets.size() <= pipelineID) {
			descriptorSets.resize(pipelineID + 1);
		}
		if (descriptorSets[pipelineID].size() <= set) {
			descriptorSets[pipelineID].resize(set + 1);
		}
		if (descriptorSets[pipelineID][set].size() <= threadID) {
			descriptorSets[pipelineID][set].resize(threadID + 1, VK_NULL_HANDLE);
		}

		VkDescriptorSetLayout layouts[] = { layout };
		renderer->GetDescriptorPool(threadID)->AllocateDescriptorSet(1, layouts, &descriptorSets[pipelineID][set][threadID]);

		VkDescriptorType type;
		VkImageLayout imageLayout;
		if (storage) {
			type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		} else {
			type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		renderer->GetDescriptorPool(threadID)->DescriptorSetBindToImage(descriptorSets[pipelineID][set][threadID],
			imageLayout,
			imageView, sampler, 0, 0,
			type, 1);

		return descriptorSets[pipelineID][set][threadID];
	}
	VkDescriptorSet VulkanTexture::GetDescriptorSet(size_t threadID, size_t pipelineID, size_t set) {
		if (pipelineID >= descriptorSets.size())
			return VK_NULL_HANDLE;
		if (set >= descriptorSets[pipelineID].size())
			return VK_NULL_HANDLE;
		if (threadID >= descriptorSets[pipelineID][set].size())
			return VK_NULL_HANDLE;
		return descriptorSets[pipelineID][set][threadID];
	}
}
#endif