#pragma once
#include "Engine/Utility/Defines.hpp"

#ifdef USING_VULKAN
#include "Engine/Platform/Vulkan/Material/VulkanMaterial.hpp"
#include "Engine/Platform/Vulkan/Mesh/VulkanMesh.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanBuffer.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanDescriptorPool.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanLogicalDevice.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanPipeline.hpp"
#include "Engine/Platform/Vulkan/Texture/VulkanTexture.hpp"

#include <glm/glm.hpp>
#include <map>
#include <vulkan/vulkan.h>

namespace Engine {

	class VulkanRenderer;


	class VulkanStaticMeshRenderer
	{
	public:
		VulkanStaticMeshRenderer(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool);
		~VulkanStaticMeshRenderer();

		void StartRender(glm::mat4 view, glm::mat4 projection);

		void RenderMesh(const glm::mat4x4& modelMatrix, std::shared_ptr<VulkanMesh> mesh,
			std::shared_ptr<VulkanMaterial> material, const glm::vec4& mainColor = glm::vec4(1.f, 1.f, 1.f, 1.f));

		void FinishRender(size_t threadID, VkCommandPool commandPool, VkCommandBuffer buffer);

		void RenderShadows(size_t threadID, VkCommandPool commandPool, VkCommandBuffer buffer, uint32_t lightOffset);

		void Clean() const;
		void Recreate();

		const int MAX_INSTANCE_COUNT = 1024;

	protected:

		std::unique_ptr<VulkanPipeline> staticMeshPipeline_;
		std::unique_ptr<VulkanPipeline> shadowPipeline_;

		std::unique_ptr<VulkanBuffer> uniformBuffer_;

		typedef struct {
			glm::mat4 view;
			glm::mat4 proj;
		}Ubo_t;

		Ubo_t ubo_;

		std::shared_ptr<VulkanTexture> defaultTexture_;

		std::vector<VkDescriptorSet> uboDescriptors_;

		typedef struct {
			glm::mat4 model;
			glm::vec4 color;
		}PushConstants_t;

		struct MeshData {
			std::vector<glm::mat4> transforms;
			std::unique_ptr<VulkanBuffer> transformBuffer;
			VkDescriptorSet materialDescriptor;
			std::shared_ptr<VulkanMaterial> material;
			int meshes;
		};

		std::multimap<std::shared_ptr<VulkanMesh>, MeshData*> meshInstances_;

		VulkanRenderer* renderer_;
		VulkanLogicalDevice* device_;
		VulkanDescriptorPool* descriptorPool_;

		VmaAllocator allocator_;

		VkCommandPool commandPool_;

		VkCommandBuffer commandBuffer_;
	};

}

#endif // USING_VULKAN