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

	class VulkanSpriteRenderer
	{
	public:
		VulkanSpriteRenderer(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool);
		~VulkanSpriteRenderer();

		void StartRender(VkCommandBuffer renderPassBuffer, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

		void RenderSprite(std::weak_ptr<VulkanTexture> texture, glm::mat4 modelMatrix);

		void FinishRender();

		void Clean() const;

		void Recreate();

	protected:
		std::unique_ptr<VulkanPipeline> renderPassPipeline;

		std::unique_ptr<VulkanBuffer> uboBuffer;
		std::unique_ptr<VulkanBuffer> vertexBuffer;
		std::unique_ptr<VulkanBuffer> indexBuffer;

		std::vector<uint32_t> indices;
		
		typedef struct {
			glm::vec3 position;
			glm::vec2 texCoord;
		}VertexInfo_t;

		typedef struct {
			glm::mat4 view;
			glm::mat4 proj;
		}Ubo_t;

		Ubo_t ubo;

		typedef struct {
			glm::mat4 model;
		}PushConstants_t;

		VulkanRenderer* renderer;
		VulkanLogicalDevice* device;
		VulkanDescriptorPool* descriptorPool;

		VkDescriptorSet renderUboDescriptorSet;

		VkCommandBuffer renderCommandBuffer;

		
	};

}

#endif // USING_VULKAN