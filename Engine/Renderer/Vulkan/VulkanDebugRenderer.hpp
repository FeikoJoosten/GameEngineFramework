#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Texture/VulkanTexture.hpp"
#include "Engine/Renderer/Vulkan/VulkanBuffer.hpp"
#include "Engine/Renderer/Vulkan/VulkanDescriptorPool.hpp"
#include <glm/glm.hpp>
#include <memory>

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