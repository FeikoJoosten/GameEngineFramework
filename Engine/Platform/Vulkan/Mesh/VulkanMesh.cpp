#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include "Engine/Engine/Engine.hpp"
#include "Engine/Platform/Vulkan/Mesh/VulkanMesh.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanRenderer.hpp"
#include "Engine/Platform/Vulkan/Texture/VulkanTexture.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanRenderer.hpp"
#include "Engine/Utility/Logging.hpp"

#include <assimp/types.h>

namespace Engine
{

	VulkanRenderer* VulkanMesh::renderer = nullptr;
	VulkanLogicalDevice* VulkanMesh::device = nullptr;
	VulkanDescriptorPool* VulkanMesh::descriptorPool = nullptr;
	VkCommandPool VulkanMesh::commandPool = VK_NULL_HANDLE;
	VmaAllocator VulkanMesh::allocator = VK_NULL_HANDLE;

	void VulkanMesh::InitMeshes(VulkanRenderer * renderer, VulkanLogicalDevice * device, VulkanDescriptorPool* descriptorPool, VkCommandPool commandPool)
	{
		VulkanMesh::renderer = renderer;
		VulkanMesh::device = device;
		VulkanMesh::descriptorPool = descriptorPool;
		VulkanMesh::commandPool = commandPool;
		VulkanMesh::allocator = renderer->GetVmaAllocator();
	}

	VulkanMesh::VulkanMesh(aiMesh * mesh, std::shared_ptr<Skeleton> skeleton, std::vector<Vertex> vertices, std::vector<unsigned> indices) : Mesh(vertices, indices)
	{
		this->mesh = mesh;
		this->skeleton = skeleton;

		SetUpMesh();
	}

	VulkanMesh::~VulkanMesh()
	{
		vertexBuffer.reset();
		indexBuffer.reset();
	}

	void VulkanMesh::SetUpMesh()
	{
		//std::weak_ptr<VulkanRenderer> vulkanRenderer = Engine::GetRenderer<VulkanRenderer>();
		//VkBuffer VBOBuffer, EBOBuffer, UBOBuffer;

		//vulkanRenderer->CreateBuffer<Vertex>(vertices, VBOBuffer, vertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT); //Creates a vertex buffer on GPU
		//vulkanRenderer->CreateBuffer<uint32_t>(indices, EBOBuffer, indexBufferMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT); // Creates Index buffer on GPU
		//
		//vulkanRenderer->CreateUniformBuffer(&UBOBuffer, &uniformBufferMemory); //Sends uniforms each frame to the GPU

		//vbo = uint64_t(VBOBuffer);
		//ebo = uint64_t(EBOBuffer);
		//ubo = uint64_t(UBOBuffer);

		animated = false;

		boneOffsets.resize(255, glm::mat4());

		if (mesh->HasBones() && skeleton != nullptr) {
			//skeletal mesh, load bones
			std::map<std::string, Skeleton::Bone_t*> boneMap = skeleton->GetBoneMap();

			for (size_t i = 0, size = mesh->mNumBones; i < size; ++i) {
				if (boneMap.find(std::string(mesh->mBones[i]->mName.C_Str())) == boneMap.end()) {
					DEBUG_WARNING("Mesh references bone " + std::string(mesh->mBones[i]->mName.C_Str()) + " Which isn't found in skeleton " + skeleton->GetName());
				}
				int index = boneMap[std::string(mesh->mBones[i]->mName.C_Str())]->boneDataIndex;
				aiBone* bone = mesh->mBones[i];

				boneOffsets[index] = glm::mat4(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
					bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
					bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
					bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4);

				for (size_t j = 0; j < bone->mNumWeights; ++j) {
					size_t id = 0;
					float smallestWeight = vertices[bone->mWeights[j].mVertexId].boneWeights[id];

					for (size_t k = 0; k < 4; ++k) {
						if (vertices[bone->mWeights[j].mVertexId].boneWeights[k] < smallestWeight) {
							id = k;
							smallestWeight = vertices[bone->mWeights[j].mVertexId].boneWeights[k];
						}
					}

					if (smallestWeight < bone->mWeights[j].mWeight) {
						vertices[bone->mWeights[j].mVertexId].boneIds[id] = index;
						vertices[bone->mWeights[j].mVertexId].boneWeights[id] = bone->mWeights[j].mWeight;
					}

				}
			}

			animated = true;
		}

		vertexBuffer = std::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, allocator,
			static_cast<uint32_t>(sizeof(Vertex)*vertices.size()), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, true, commandPool));

		indexBuffer = std::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, allocator,
			static_cast<uint32_t>(sizeof(uint32_t)*indices.size()), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, true, commandPool));

		offsetBuffer = std::unique_ptr <VulkanBuffer>(new VulkanBuffer(device, allocator,
			static_cast<uint32_t>(sizeof(glm::mat4) * 255), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true, commandPool));

		std::map<aiVector3D, uint32_t> positionMap;
		std::map<Edge, std::vector<Face>> neighbors;

		std::vector<Face> uniqueFaces;

		// Calculate adjacencies
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];

			Face unique;

			for (int j = 0; j < 3; ++j) {
				uint32_t index = static_cast<uint32_t>(face.mIndices[j]);

				aiVector3D v = mesh->mVertices[index];

				if (positionMap.find(v) == positionMap.end()) {
					positionMap[v] = index;
				}
				else {
					index = positionMap[v];
				}

				unique.indices[j] = index;

			}

			uniqueFaces.push_back(unique);

			Edge edge1(unique.indices[0], unique.indices[1]);
			Edge edge2(unique.indices[1], unique.indices[2]);
			Edge edge3(unique.indices[2], unique.indices[0]);

			neighbors[edge1].push_back(unique);
			neighbors[edge2].push_back(unique);
			neighbors[edge3].push_back(unique);
		}

		std::vector<uint32_t> intIndices(indices.size());

		for (size_t i = 0, size = indices.size(); i < size; ++i) {
			intIndices[i] = static_cast<uint32_t>(indices[i]);
		}

		std::vector<uint32_t> shadowIndices(uniqueFaces.size() * 6);

		shadowIndicesCount = static_cast<uint32_t>(uniqueFaces.size() * 6);

		for (size_t i = 0, size = uniqueFaces.size(); i < size; ++i) {
			Face face = uniqueFaces[i];

			Edge edge1(face.indices[0], face.indices[1]);
			Edge edge2(face.indices[1], face.indices[2]);
			Edge edge3(face.indices[2], face.indices[0]);

			Face neighbor1;
			Face neighbor2;
			Face neighbor3;

			std::vector<Face> edge1Neighbors = neighbors[edge1];
			std::vector<Face> edge2Neighbors = neighbors[edge2];
			std::vector<Face> edge3Neighbors = neighbors[edge3];

			if (edge1Neighbors.size() < 2)
				neighbor1 = face;
			else
				neighbor1 = edge1Neighbors[0] == face ? edge1Neighbors[1] : edge1Neighbors[0];
			if (edge2Neighbors.size() < 2)
				neighbor2 = face;
			else
				neighbor2 = edge2Neighbors[0] == face ? edge2Neighbors[1] : edge2Neighbors[0];
			if (edge3Neighbors.size() < 2)
				neighbor3 = face;
			else
				neighbor3 = edge3Neighbors[0] == face ? edge3Neighbors[1] : edge3Neighbors[0];

			shadowIndices[i * 6 + 0] = static_cast<uint32_t>(face.indices[0]);

			if (neighbor1 == face)
				shadowIndices[i * 6 + 1] = face.indices[2];
			else
				shadowIndices[i * 6 + 1] = neighbor1.FindOpposingIndex(edge1);

			shadowIndices[i * 6 + 2] = static_cast<uint32_t>(face.indices[1]);

			if (neighbor2 == face)
				shadowIndices[i * 6 + 3] = face.indices[0];
			else
				shadowIndices[i * 6 + 3] = neighbor2.FindOpposingIndex(edge2);

			shadowIndices[i * 6 + 4] = static_cast<uint32_t>(face.indices[2]);

			if (neighbor3 == face)
				shadowIndices[i * 6 + 5] = face.indices[1];
			else
				shadowIndices[i * 6 + 5] = neighbor3.FindOpposingIndex(edge3);
		}

		shadowIndexBuffer = std::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, allocator,
			static_cast<uint32_t>(sizeof(uint32_t)*shadowIndicesCount),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT, true, commandPool));

		vertexBuffer->UpdateBuffer(vertices.data(), 0, static_cast<uint32_t>(sizeof(Vertex)*vertices.size()));
		indexBuffer->UpdateBuffer(intIndices.data(), 0, static_cast<uint32_t>(sizeof(uint32_t)*intIndices.size()));
		offsetBuffer->UpdateBuffer(boneOffsets.data(), 0, static_cast<uint32_t>(sizeof(glm::mat4) * 255));

		shadowIndexBuffer->UpdateBuffer(shadowIndices.data(), 0,
			static_cast<uint32_t>(sizeof(uint32_t)*shadowIndicesCount));

	}

	VkBuffer VulkanMesh::GetVertexBuffer() const
	{
		return vertexBuffer->GetBuffer();
	}

	VkBuffer VulkanMesh::GetIndexBuffer() const
	{
		return indexBuffer->GetBuffer();
	}

	VkBuffer VulkanMesh::GetShadowIndexBuffer() const
	{
		return shadowIndexBuffer->GetBuffer();
	}

	uint32_t VulkanMesh::GetShadowIndexCount() const
	{
		return shadowIndicesCount;
	}

	uint32_t VulkanMesh::GetIndexCount()
	{
		return static_cast<uint32_t>(indices.size());
	}

	bool VulkanMesh::IsAnimated() const
	{
		return animated;
	}

	VkDescriptorSet VulkanMesh::CreateBoneOffsetDescriptorSet(size_t threadID, size_t pipelineID, size_t set, VkDescriptorSetLayout layout)
	{
		if (offsetDescriptorSets.size() <= pipelineID) {
			offsetDescriptorSets.resize(pipelineID + 1);
		}
		if (offsetDescriptorSets[pipelineID].size() <= set) {
			offsetDescriptorSets[pipelineID].resize(set + 1);
		}
		if (offsetDescriptorSets[pipelineID][set].size() <= threadID) {
			offsetDescriptorSets[pipelineID][set].resize(threadID + 1, VK_NULL_HANDLE);
		}

		VkDescriptorSetLayout layouts[] = { layout };
		renderer->GetDescriptorPool(threadID)->AllocateDescriptorSet(1, layouts, &offsetDescriptorSets[pipelineID][set][threadID]);

		renderer->GetDescriptorPool(threadID)->DescriptorSetBindToBuffer(offsetDescriptorSets[pipelineID][set][threadID], offsetBuffer->GetBuffer(),
			0, static_cast<VkDeviceSize>(sizeof(glm::mat4) * 255), 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);

		return offsetDescriptorSets[pipelineID][set][threadID];
	}

	VkDescriptorSet VulkanMesh::GetBoneOffsetDescriptorSet(size_t threadID, size_t pipelineID, size_t set)
	{
		if (pipelineID >= offsetDescriptorSets.size())
			return VK_NULL_HANDLE;
		if (set >= offsetDescriptorSets[pipelineID].size())
			return VK_NULL_HANDLE;
		if (threadID >= offsetDescriptorSets[pipelineID][set].size())
			return VK_NULL_HANDLE;
		return offsetDescriptorSets[pipelineID][set][threadID];
	}
} //namespace Engine
#endif