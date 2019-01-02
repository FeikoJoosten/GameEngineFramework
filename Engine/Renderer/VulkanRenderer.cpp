// Written by Koen Buitenhuis

#include "Engine/Renderer/VulkanRenderer.hpp"
#include "Engine/Renderer/imgui_impl_glfw_vulkan.h"
#include "Engine/engine.hpp"
#include "Engine/Utility/Utility.hpp"
#include <chrono>
#include <cstring>
#include <ThirdParty/EASTL-master/include/EASTL/set.h>
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>

#if !defined STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>
#include "Engine/Mesh/VulkanMesh.hpp"

namespace Engine
{

	VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
	{
		PFN_vkCreateDebugReportCallbackEXT func = PFN_vkCreateDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
		return func != nullptr ? func(instance, pCreateInfo, pAllocator, pCallback) : VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
	{
		PFN_vkDestroyDebugReportCallbackEXT func = PFN_vkDestroyDebugReportCallbackEXT(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
		if (func != nullptr)
		{
			func(instance, callback, pAllocator);
		}
	}

	VulkanRenderer::VulkanRenderer() noexcept :	instance(nullptr), callback(0), descriptorSetLayout(0),
		pipelineLayout(0), descriptorPool(0), device(nullptr),
		graphicsQueue(nullptr), presentQueue(nullptr), commandPool(0), imageAvailableSemaphore(0),
		renderFinishedSemaphore(0)
	{
	}

	void VulkanRenderer::InitializeRenderer()
	{
		InitVulkan();
		SetupIMGUI();
		
		std::cout << "renderer Initialized.." << std::endl;
	}
	
	void VulkanRenderer::RendererBegin(const glm::mat4x4 & view, const glm::mat4x4 & projection)
	{
		ImGui_ImplGlfwVulkan_NewFrame();
		VkResult result = vkAcquireNextImageKHR(device, Engine::GetWindow<VulkanWindow>()->GetSwapChain(), eastl::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			EASTL_ASSERT_MSG(true, "Failed to acquire swap chain image!");
			return;
		}

		//Main recording part
		VkCommandBuffer drawCommandBuffer = BeginFrame(int(imageIndex));
		MVP.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));//view;
		MVP.projection = glm::perspective(
			glm::radians(45.0f), 
			Engine::GetWindow<VulkanWindow>()->GetSwapChainExtent().width / static_cast<float>(Engine::GetWindow<VulkanWindow>()->GetSwapChainExtent().height),
			0.1f, 10.0f); //projection;
	}

	void VulkanRenderer::Render(const glm::mat4x4& modelMatrix, Model* model, const glm::vec4& mainColor)
	{
		MVP.model = modelMatrix;
		
		eastl::vector<Mesh*>& meshes = model->GetModelMeshes();
		for (size_t i = 0, size = meshes.size(); i < size; ++i)
		{
			VulkanMesh* vulkanmesh = static_cast<VulkanMesh*>(meshes[i]);
			UpdateUniformBuffer(vulkanmesh->getUniformMemory());
			VkDescriptorSet* Dset = dynamic_cast<VulkanTexture*>(vulkanmesh->GetTextures()[0])->getDescriptorSet();
			//Binding the vertex buffer to the pipeline
			VkBuffer vertexBuffers[] = { VkBuffer(reinterpret_cast<VkBuffer>(meshes[i]->GetVBO())) };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(myCommandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(myCommandBuffers[imageIndex], VkBuffer(reinterpret_cast<VkBuffer>(meshes[i]->GetEBO())), 0, VK_INDEX_TYPE_UINT32);
			
			vkCmdBindDescriptorSets(myCommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, Dset, 0, nullptr);
			
			vkCmdDrawIndexed(myCommandBuffers[imageIndex], static_cast<uint32_t>(meshes[i]->indices.size()), 1, 0, 0, 0);
		}
	}

	void VulkanRenderer::RendererEnd()
	{
	
		ImGui_ImplGlfwVulkan_Render(myCommandBuffers[imageIndex]);
		EndFrame(int(imageIndex));
		//End of recording

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &myCommandBuffers[imageIndex];


		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to submit draw command buffer!");
			return;
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { Engine::GetWindow<VulkanWindow>()->GetSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(presentQueue, &presentInfo);

		vkQueueWaitIdle(presentQueue);
	}

	void VulkanRenderer::InitVulkan()
	{
		CreateInstance();
		SetupDebugCallback();
		Engine::GetWindow<VulkanWindow>()->InitSurface(&instance);
		PickPhysicalDevice();
		CreateLogicalDevice();
		Engine::GetWindow<VulkanWindow>()->CreateSwapChain(&device, &physicalDevice);
		Engine::GetWindow<VulkanWindow>()->CreateImageViews(&device);
		Engine::GetWindow<VulkanWindow>()->CreateRenderPass(&device, &physicalDevice);
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline("Resources/Shaders/vert.spv", "Resources/Shaders/frag.spv");
		CreateCommandPool();
		Engine::GetWindow<VulkanWindow>()->CreateDepthRecources(&device, &physicalDevice);
		Engine::GetWindow<VulkanWindow>()->CreateSwapChainFrameBuffers(&device);
		CreateDescriptorPool();
		CreatePrivateCmdBuffers();
		CreateSemaphores();
	}

	void VulkanRenderer::CreateModel(eastl::vector<Vertex> theVertices, eastl::vector<unsigned int> theIndices, unsigned int VBO, unsigned int EBO, unsigned int UBO)
	{
		/*//For each model:
		CreateBuffer<Vertex>(theVertices, m_VertexBuffer, m_VertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT); //Creates a vertex buffer on GPU
		vertexBuffers.push_back(&m_VertexBuffer);
		CreateBuffer<uint32_t>(theIndices, m_IndexBuffer, m_IndexBufferMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT); // Creates Index buffer on GPU
		indexBuffers.push_back(&m_IndexBuffer);
		CreateUniformBuffer(&m_UniformBuffer, &m_UniformBufferMemory); //Sends uniforms each frame to the GPU
		uniformBuffers.push_back(&m_UniformBuffer);
		uniformBuffersMemory.push_back(&m_UniformBufferMemory);
		CreateDescriptorSet(&m_DescriptorSet, &m_UniformBuffer, &m_TextureImageView, &m_TextureSampler);
		/End*/
	}

	void VulkanRenderer::RecreateSwapChain()
	{
		vkDeviceWaitIdle(device);

		CleanupSwapChain();

		Engine::GetWindow<VulkanWindow>()->CreateSwapChain(&device, &physicalDevice);
		Engine::GetWindow<VulkanWindow>()->CreateImageViews(&device);
		Engine::GetWindow<VulkanWindow>()->CreateRenderPass(&device, &physicalDevice);
		char* path1 = "Resources/Shaders/vert.spv"; char* path2 = "Resources/Shaders/frag.spv";
		CreateGraphicsPipeline(path1, path2);
		Engine::GetWindow<VulkanWindow>()->CreateDepthRecources(&device, &physicalDevice);
		Engine::GetWindow<VulkanWindow>()->CreateSwapChainFrameBuffers(&device);
		//CreateCommandBuffers();
		CreatePrivateCmdBuffers();
	}

	void VulkanRenderer::CleanupSwapChain()
	{
		Engine::GetWindow<VulkanWindow>()->DestroyDepthRecourses(&device);
		Engine::GetWindow<VulkanWindow>()->DestroyFrameBuffers(&device);

		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		for (size_t i = 0, end = graphicsPipelines.size(); i < end; i++)
		{
			vkDestroyPipeline(device, graphicsPipelines[i], nullptr);
		}
		graphicsPipelines.resize(0);
		std::cout << graphicsPipelines.size() << " : is now the size of pipeline vector" << std::endl;
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, Engine::GetWindow<VulkanWindow>()->GetRenderPass(), nullptr);

		Engine::GetWindow<VulkanWindow>()->DestroyImageViews(&device);

		vkDestroySwapchainKHR(device, Engine::GetWindow<VulkanWindow>()->GetSwapChain(), nullptr);
	}

	void VulkanRenderer::SetupIMGUI()
	{
		// Setup ImGui binding
		ImGui_ImplGlfwVulkan_Init_Data init_data = {};
		init_data.allocator = nullptr;
		init_data.gpu = physicalDevice;
		init_data.device = device;
		init_data.render_pass = Engine::GetWindow<VulkanWindow>()->GetRenderPass();
		init_data.pipeline_cache = VK_NULL_HANDLE;
		init_data.descriptor_pool = descriptorPool;
		//init_data.check_vk_result = nullptr;
		ImGui_ImplGlfwVulkan_Init(Engine::GetWindow<VulkanWindow>()->GetGLFWWindow(), &init_data);

		VkCommandBuffer UploadFonts = BeginSingleTimeCommands();
		ImGui_ImplGlfwVulkan_CreateFontsTexture(UploadFonts);
		EndSingleTimeCommands(UploadFonts);
		vkDeviceWaitIdle(device);
		ImGui_ImplGlfwVulkan_InvalidateFontUploadObjects();
	}

	void VulkanRenderer::UpdateUniformBuffer(VkDeviceMemory uniformBufferMemory)
	{
		float time = timepassed += Engine::GetTime().GetDeltaTime();
		UniformBufferObject ubo;
		
		ubo.model = MVP.model;
		//ubo.model += glm::rotate(glm::mat4(1.0f), time / 100 * glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.projection = MVP.projection;
		ubo.view = MVP.view;
		//ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//ubo.projection = glm::perspective(glm::radians(45.0f), Engine::Engine::GetWindow<Engine::VulkanWindow>()->GetSwapChainExtent().width / float(Engine::Engine::GetWindow<Engine::VulkanWindow>()->GetSwapChainExtent().height), 0.1f, 10.0f);
		ubo.projection[1][1] *= -1;

		void* data;
		vkMapMemory(device, uniformBufferMemory, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, uniformBufferMemory);
	}
	
	void VulkanRenderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to create semaphores!");
		}
	}

	void VulkanRenderer::CreateUniformBuffer(VkBuffer* uniformBuffer, VkDeviceMemory* uniformBufferMemory)
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *uniformBuffer, *uniformBufferMemory);
	}

	void VulkanRenderer::CreateDescriptorPool()
	{
		VkDescriptorPoolSize pool_size[2] =
		{
			//{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			//{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			//{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			//{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			//{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 }
			//{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			//{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			//{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			//{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		}; //May oncomment other discritors if needed later on, make sure to set maxsets and poolsizecount to the correct numbers!

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000 * 2;
		poolInfo.poolSizeCount = 2;
		poolInfo.pPoolSizes = pool_size;

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "Failed to create a descriptor pool!");
		}
	}

	void VulkanRenderer::CreateDescriptorSet(VkDescriptorSet* descriptorSet, VkBuffer* uniformBuffer, VkImageView* imageView, VkSampler* textureSampler)
	{
		VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts;

		if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSet) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to allocate descriptor set!");
			return;
		}

		VkDescriptorBufferInfo bufferInfo;
		bufferInfo.buffer = *uniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = *imageView;
		imageInfo.sampler = *textureSampler;

		eastl::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = *descriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = *descriptorSet;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "Failed to create the vertex buffer!");
			return;
		}

		VkMemoryRequirements memRequirements = {};
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "Failed to allocate memory for vertex buffer!");
			return;
		}
		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(commandBuffer);
	}

	void VulkanRenderer::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "Failed to create Image!");
			return;
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "Failed to allocate image memory!");
			return;
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	VkCommandBuffer VulkanRenderer::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanRenderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	void VulkanRenderer::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else {
			EASTL_ASSERT_MSG(true, "unsupported layout transition!");
			return;
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		EndSingleTimeCommands(commandBuffer);
	}

	void VulkanRenderer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy region;

		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		EndSingleTimeCommands(commandBuffer);
	}

	void VulkanRenderer::CleanupRenderer()
	{
		vkDeviceWaitIdle(device);
		CleanupSwapChain();

		//vkDestroySampler(device, m_TextureSampler, nullptr);
		//vkDestroyImageView(device, m_TextureImageView, nullptr);

		//vkDestroyImage(device, m_TextureImage, nullptr);
		//vkFreeMemory(device, m_TextureImageMemory, nullptr);

		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		//vkDestroyBuffer(device, m_UniformBuffer, nullptr);
		//vkFreeMemory(device, m_UniformBufferMemory, nullptr);

		//vkDestroyBuffer(device, vertexBuffer, nullptr);
		//vkFreeMemory(device, vertexBufferMemory, nullptr);
		//vkDestroyBuffer(device, indexBuffer, nullptr);
		//vkFreeMemory(device, indexBufferMemory, nullptr);

		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);

		vkDestroyCommandPool(device, commandPool, nullptr);

		vkDestroyDevice(device, nullptr);
		DestroyDebugReportCallbackEXT(instance, callback, nullptr);
		Engine::GetWindow<VulkanWindow>()->DestroySurface(&instance);
		vkDestroyInstance(instance, nullptr);
		//ImGui_ImplGlfwVulkan_Shutdown();
	}

	void VulkanRenderer::Render()
	{
	}

	void VulkanRenderer::CreateCommandPool() {
		QueueFamilyIndices queueFamilyIndices = Engine::GetWindow<VulkanWindow>()->FindQueueFamilies(&physicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to create command pool!");
		}
	}

	void VulkanRenderer::CreateTextureImage(int texWidth, int texHeight, unsigned char* pixels, VkImage* textureImage, VkDeviceMemory* textureImageMemory)
	{
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels)
		{
			EASTL_ASSERT_MSG(true, "Could not find the texture image!");
			return;
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device, stagingBufferMemory);

		CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *textureImage, *textureImageMemory);

		TransitionImageLayout(*textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer, *textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		TransitionImageLayout(*textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void VulkanRenderer::CreateTextureImage()
	{
	}

	void VulkanRenderer::CreateTextureSampler(VkSampler* sampler, bool isFont)
	{
		VkSamplerCreateInfo samplerInfo = {};
		if (!isFont)
		{
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.anisotropyEnable = VK_TRUE;
			samplerInfo.maxAnisotropy = 16;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;
		}
		else
		{
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.minLod = -1000;
			samplerInfo.maxLod = 1000;
			samplerInfo.maxAnisotropy = 1.0f;
		}

		if (vkCreateSampler(device, &samplerInfo, nullptr, sampler) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to create texture sampler!");
		}
	}

	void VulkanRenderer::CreateTextureImageView()
	{
		//m_TextureImageView = CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_UNORM, &device, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanRenderer::CreateTextureImageView(VkImageView* textureImageView, VkImage* textureImage)
	{
		*textureImageView = CreateImageView(*textureImage, VK_FORMAT_R8G8B8A8_UNORM, &device, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	VkImageView VulkanRenderer::CreateImageView(VkImage image, VkFormat format, VkDevice* device, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(*device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to create texture image view!");
			return nullptr;
		}

		return imageView;
	}

	VkCommandBuffer VulkanRenderer::BeginFrame(int imageIndex)
	{
		vkResetCommandBuffer(myCommandBuffers[imageIndex], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(myCommandBuffers[imageIndex], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = Engine::GetWindow<VulkanWindow>()->GetRenderPass();
		renderPassInfo.framebuffer = Engine::GetWindow<VulkanWindow>()->GetSwapChainFramebuffers()[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = Engine::GetWindow<VulkanWindow>()->GetSwapChainExtent();

		eastl::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(myCommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		//As long as we dont have any other shaders, pipeline binding can stay in begin frame!

		vkCmdBindPipeline(myCommandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelines[0]);
		return myCommandBuffers[imageIndex];
	}

	void VulkanRenderer::EndFrame(int imageIndex)
	{
		vkCmdEndRenderPass(myCommandBuffers[imageIndex]);
		vkEndCommandBuffer(myCommandBuffers[imageIndex]);
	}

	void VulkanRenderer::CreatePrivateCmdBuffers()
	{
		myCommandBuffers.resize(Engine::GetWindow<VulkanWindow>()->GetSwapChainFramebuffers().size());
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = uint32_t(myCommandBuffers.size());

		if (vkAllocateCommandBuffers(device, &allocInfo, myCommandBuffers.data()) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to allocate command buffers!");
		}
	}

	//Disabled
	void VulkanRenderer::CreateCommandBuffers()
	{
	}

	void VulkanRenderer::CreateInstance()
	{
		if (enableValidationLayers && !CheckValidationLayerSupport())
		{
			EASTL_ASSERT_MSG(true, "validation layers requested, but not available!");
			return;
		}

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to create instance!");
		}
	}

	void VulkanRenderer::SetupDebugCallback()
	{
		if (!enableValidationLayers) return;

		VkDebugReportCallbackCreateInfoEXT createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = DebugCallback;

		if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to set up debug callback!");
		}
	}

	void VulkanRenderer::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			EASTL_ASSERT_MSG(true, "failed to find GPUs with Vulkan support!");
			return;
		}

		eastl::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE)
		{
			EASTL_ASSERT_MSG(true, "failed to find a suitable GPU!");
		}
	}

	void VulkanRenderer::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = Engine::GetWindow<VulkanWindow>()->FindQueueFamilies(&physicalDevice);

		eastl::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		eastl::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

		float queuePriority = 1.0f;
		for (int queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};

		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to create logical device!");
			return;
		}

		vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
	}

	void VulkanRenderer::CreateGraphicsPipeline(char* vertPath, char* fragPath)
	{
		auto vertShaderCode = Utility::ReadFile(vertPath, std::ios::ate | std::ios::binary);
		auto fragShaderCode = Utility::ReadFile(fragPath, std::ios::ate | std::ios::binary);

		std::cout << "Size of buffer: " << sizeof(vertShaderCode) << std::endl;

		VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		auto bindingDescription = GetBindingDescription();
		auto attributeDescriptions = GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = float(Engine::GetWindow<VulkanWindow>()->GetSwapChainExtent().width);
		viewport.height = float(Engine::GetWindow<VulkanWindow>()->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = Engine::GetWindow<VulkanWindow>()->GetSwapChainExtent();

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional



		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to create pipeline layout!");
			return;
		}

		//Creating the pipeline itself
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional

		pipelineInfo.layout = pipelineLayout;

		pipelineInfo.renderPass = Engine::GetWindow<VulkanWindow>()->GetRenderPass();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		VkPipeline thePipeLine;
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &thePipeLine) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "failed to create graphics pipeline!");
			return;
		}
		graphicsPipelines.push_back(thePipeLine);
		std::cout << "Pipeline vector now at size " << graphicsPipelines.size() << "." << std::endl;

		//Destorying the shader modules after pipeline creation.
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
	}

	VkShaderModule VulkanRenderer::CreateShaderModule(const eastl::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "Failed to create the shader module.");
			return nullptr;
		}
		return shaderModule;
	}

	bool VulkanRenderer::IsDeviceSuitable(VkPhysicalDevice physicalDevice)
	{
		QueueFamilyIndices queueFamilyIndices = Engine::GetWindow<VulkanWindow>()->FindQueueFamilies(&physicalDevice);

		bool extensionsSupported = CheckDeviceExtensionSupport(physicalDevice);

		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = Engine::GetWindow<VulkanWindow>()->QuerySwapChainSupport(&physicalDevice);
			bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

		return queueFamilyIndices.isComplete() && extensionsSupported && supportedFeatures.samplerAnisotropy;
	}

	bool VulkanRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		eastl::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		eastl::set<eastl::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	void VulkanRenderer::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding;
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; //Can be set to be used in all shaders, for example.
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding;
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		eastl::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			EASTL_ASSERT_MSG(true, "Failed to create descriptor layout");
		}
	}

	uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		EASTL_ASSERT_MSG(true, "failed to find suitable memory type!");

		return 0;
	}

	eastl::vector<const char*> VulkanRenderer::GetRequiredExtensions()
	{
		eastl::vector<const char*> extensions;

		unsigned int glfwExtensionCount = 0;
		const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		for (unsigned int i = 0; i < glfwExtensionCount; i++)
		{
			extensions.push_back(glfwExtensions[i]);
		}

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		return extensions;
	}

	bool VulkanRenderer::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		eastl::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}
} //namespace Engine