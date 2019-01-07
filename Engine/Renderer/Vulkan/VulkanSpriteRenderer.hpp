#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN

#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>

#include "Engine/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Texture/VulkanTexture.hpp"
#include "Engine/Renderer/Vulkan/VulkanBuffer.hpp"
#include "Engine/Renderer/Vulkan/VulkanDescriptorPool.hpp"
#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>

namespace Engine {

	class VulkanRenderer;

	class VulkanSpriteRenderer
	{
	public:
		VulkanSpriteRenderer(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool);
		~VulkanSpriteRenderer();

		void StartRender(VkCommandBuffer renderPassBuffer, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

		void RenderSprite(eastl::weak_ptr<VulkanTexture> texture, glm::mat4 modelMatrix);

		void FinishRender();

		void Clean() const;

		void Recreate();

	protected:
		eastl::unique_ptr<VulkanPipeline> renderPassPipeline;

		eastl::unique_ptr<VulkanBuffer> uboBuffer;
		eastl::unique_ptr<VulkanBuffer> vertexBuffer;
		eastl::unique_ptr<VulkanBuffer> indexBuffer;

		eastl::vector<uint32_t> indices;
		
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