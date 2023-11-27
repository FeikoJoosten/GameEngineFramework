#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include "Engine/Api/Api.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Texture/VulkanTexture.hpp"
#include "Engine/Renderer/Vulkan/VulkanBuffer.hpp"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Renderer/Vulkan/VulkanDescriptorPool.hpp"
#include "Engine/Renderer/Vulkan/vk_mem_alloc.h"
#include "Engine/Animation/Skeleton.hpp"

#include <map>
#include <tuple>
#include <vulkan/vulkan.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace Engine
{

	class VulkanRenderer;

	/// <summary>
	/// This object is used to store data regarding a mesh. NOTE: only the resource manager is allowed to create a mesh.
	/// </summary>
	class ENGINE_LOCAL VulkanMesh : public Mesh
	{
	public:
		// Should be called from renderer
		static void InitMeshes(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool, VkCommandPool commandPool);

		/// <summary>
		/// This method allows you to load in the mesh data.
		/// </summary>
		void SetUpMesh() override;

		/// <summary>
		/// This method allows you to get the vertex buffer of this mesh.
		/// </summary>
		/// <returns>Returns the vertex buffer as VkBuffer.</returns>
		VkBuffer GetVertexBuffer() const;

		/// <summary>
		/// This method allows you to get the index buffer of this mesh.
		/// </summary>
		/// <returns>Returns the index buffer as VkBuffer.</returns>
		VkBuffer GetIndexBuffer() const;

		/// <summary>
		/// Returns an index buffer that doesn't contain duplicated vertices.
		/// </summary>
		/// <returns>A index buffer.</returns>
		VkBuffer GetShadowIndexBuffer() const;

		/// <summary>
		/// The number of indices in the buffer without duplicated vertices.
		/// </summary>
		/// <returns>A uint32 containing the number of indices.</returns>
		uint32_t GetShadowIndexCount() const;

		/// <summary>
		/// This method allows you to get the index count of this mesh.
		/// </summary>
		/// <returns>Returns the index count as an uint32_t.</returns>
		uint32_t GetIndexCount();

		/// <summary>
		/// This method allows you to check if this mesh has animations.
		/// </summary>
		/// <returns>Returns true if this mesh supports animations.</returns>
		bool IsAnimated() const;

		/// <summary>
		/// Creates a new discriptor set bound to the mesh's bone offset buffer, and configured for the given pipeline and layout.
		/// </summary>
		/// <param name="pipelineID">The pipeline the discriptor will be bound to.</param>
		/// <param name="set">The descriptor set number this descriptor will be bound to.</param>
		/// <param name="layout">The layout of the descriptor set.</param>
		/// <returns>The descriptor set that was just created.</returns>
		VkDescriptorSet CreateBoneOffsetDescriptorSet(size_t threadID, size_t pipelineID, size_t set, VkDescriptorSetLayout layout);

		/// <summary>
		/// Returns the bone offset descriptor set bound to the specified pipeline and set number.
		/// </summary>
		/// <param name="pipelineID">The pipeline this descriptor is bound to.</param>
		/// <param name="set">The set of the pipeline this descriptor is bound to.</param>
		/// <returns>The descriptor set bound to the pipeline and set number. 
		/// Returns VK_NULL_HANDLE of no descriptor is bound to the given combination.</returns>
		VkDescriptorSet GetBoneOffsetDescriptorSet(size_t threadID, size_t pipelineID, size_t set);


	private:

		VulkanMesh() = delete;
		VulkanMesh(aiMesh* mesh, std::shared_ptr<Skeleton> skeleton, std::vector<Vertex> vertices, std::vector<unsigned> indices);
		VulkanMesh(VulkanMesh const &other) = default;
	public:
		~VulkanMesh();
	private:

		//std::vector<std::shared_ptr<Texture>> textures;

		struct Edge {
			uint32_t indices[2];

			Edge(uint32_t index1, uint32_t index2) {
				if (index1 > index2) {
					indices[0] = index1;
					indices[1] = index2;
				}
				else {
					indices[0] = index2;
					indices[1] = index1;
				}
			}

			bool operator<(const Edge& right) const {
				return std::tie(indices[0], indices[1]) <
					std::tie(right.indices[0], right.indices[1]);
			}

		};

		struct Face {
			uint32_t indices[3];

			bool operator==(const Face& other)const {
				return (indices[0] == other.indices[0] &&
					indices[1] == other.indices[1] &&
					indices[2] == other.indices[2]);
			}

			uint32_t FindOpposingIndex(Edge edge) {
				for (int i = 0; i < 3; ++i) {
					if (indices[i] != edge.indices[0] && indices[i] != edge.indices[1])
						return indices[i];
				}
				return -1;
			}
		};

		std::shared_ptr<Skeleton> skeleton;

		std::vector<glm::mat4> boneOffsets;

		std::unique_ptr<VulkanBuffer> vertexBuffer;
		std::unique_ptr<VulkanBuffer> indexBuffer;
		std::unique_ptr<VulkanBuffer> offsetBuffer;

		std::unique_ptr<VulkanBuffer> shadowIndexBuffer;

		std::vector<std::vector<std::vector<VkDescriptorSet>>> offsetDescriptorSets;

		std::shared_ptr<VulkanTexture> diffuseMissing;

		const aiScene* scene;
		aiMesh* mesh;

		static VulkanRenderer* renderer;
		static VulkanLogicalDevice* device;
		static VkCommandPool commandPool;
		static VmaAllocator allocator;
		static VulkanDescriptorPool* descriptorPool;

		bool animated;

		uint32_t shadowIndicesCount;

	};

} //namespace Engine
#endif