#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN

#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>

#include "Engine/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Texture/VulkanTexture.hpp"
#include "Engine/Renderer/Vulkan/VulkanBuffer.hpp"
#include "Engine/Renderer/Vulkan/VulkanDescriptorPool.hpp"
#include "Engine/Material/VulkanMaterial.hpp"
#include "Engine/Mesh/VulkanMesh.hpp"
#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/EASTL-master/include/EASTL/map.h>

namespace Engine {

	class VulkanRenderer;


	class VulkanStaticMeshRenderer
	{
	public:
		VulkanStaticMeshRenderer(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool);
		~VulkanStaticMeshRenderer();

		void StartRender(glm::mat4 view, glm::mat4 projection);

		void RenderMesh(const glm::mat4x4& modelMatrix, eastl::shared_ptr<VulkanMesh> mesh,
			eastl::shared_ptr<VulkanMaterial> material, const glm::vec4& mainColor = glm::vec4(1.f, 1.f, 1.f, 1.f));

		void FinishRender(size_t threadID, VkCommandPool commandPool, VkCommandBuffer buffer);

		void RenderShadows(size_t threadID, VkCommandPool commandPool, VkCommandBuffer buffer, uint32_t lightOffset);

		void Clean() const;
		void Recreate();

		const int MAX_INSTANCE_COUNT = 1024;

	protected:

		eastl::unique_ptr<VulkanPipeline> staticMeshPipeline_;
		eastl::unique_ptr<VulkanPipeline> shadowPipeline_;

		eastl::unique_ptr<VulkanBuffer> uniformBuffer_;

		typedef struct {
			glm::mat4 view;
			glm::mat4 proj;
		}Ubo_t;

		Ubo_t ubo_;

		eastl::shared_ptr<VulkanTexture> defaultTexture_;

		eastl::vector<VkDescriptorSet> uboDescriptors_;

		typedef struct {
			glm::mat4 model;
			glm::vec4 color;
		}PushConstants_t;

		struct MeshData {
			eastl::vector<glm::mat4> transforms;
			eastl::unique_ptr<VulkanBuffer> transformBuffer;
			VkDescriptorSet materialDescriptor;
			eastl::shared_ptr<VulkanMaterial> material;
			int meshes;
		};

		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*> meshInstances_;

		VulkanRenderer* renderer_;
		VulkanLogicalDevice* device_;
		VulkanDescriptorPool* descriptorPool_;

		VmaAllocator allocator_;

		VkCommandPool commandPool_;

		VkCommandBuffer commandBuffer_;
	};

}

#endif // USING_VULKAN