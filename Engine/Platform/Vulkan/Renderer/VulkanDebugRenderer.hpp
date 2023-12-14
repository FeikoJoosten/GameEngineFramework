#pragma once
#include "Engine/Utility/Defines.hpp"

#ifdef USING_VULKAN
#include "Engine/Platform/Vulkan/Renderer/VulkanBuffer.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanDescriptorPool.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanLogicalDevice.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanPipeline.hpp"
#include "Engine/Platform/Vulkan/Texture/VulkanTexture.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vulkan/vulkan.h>

namespace Engine {

	class VulkanRenderer;

	class VulkanDebugRenderer
	{
	public:
		VulkanDebugRenderer(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool);
		~VulkanDebugRenderer();

		void StartRender(VkCommandBuffer commandBuffer, glm::mat4 view, glm::mat4 projection);

		void RenderLine(glm::vec3 start, glm::vec3 end, glm::vec4 color = glm::vec4(1.f,1.f,1.f,1.f));

		void FinishRender();

		void Clean();
		void Recreate();

	protected:
		std::unique_ptr<VulkanPipeline> linePipeline;

		std::unique_ptr<VulkanBuffer> vertexBuffer;
		std::unique_ptr<VulkanBuffer> uniformBuffer;

		typedef struct {
			glm::vec3 position;
		}VertexInfo_t;

		typedef struct {
			glm::mat4 view;
			glm::mat4 proj;
		}Ubo_t;

		Ubo_t ubo;

		typedef struct {
			glm::mat4 model;
			glm::vec4 color;
		}PushConstants_t;

		std::vector<PushConstants_t> lines;

		VulkanRenderer* renderer;
		VulkanLogicalDevice* device;
		VulkanDescriptorPool* descriptorPool;

		VkDescriptorSet uboDescriptorSet;

		VkCommandBuffer commandBuffer;

	};

}

#endif // USING_VULKAN