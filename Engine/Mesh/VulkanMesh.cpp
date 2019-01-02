#include "Engine/Mesh/VulkanMesh.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"
#include "Engine/Resources/ModelLoader.hpp"
#include "Engine/engine.hpp"

namespace Engine
{
	VulkanMesh::VulkanMesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices,
		eastl::vector<Texture*> textures) : Mesh(vertices, indices, textures)
	{
		VulkanMesh::SetUpMesh();
	}

	void VulkanMesh::SetUpMesh()
	{
		VulkanRenderer* vulkanRenderer = Engine::GetRenderer<VulkanRenderer>();
		VkBuffer VBOBuffer, EBOBuffer, UBOBuffer;

		vulkanRenderer->CreateBuffer<Vertex>(vertices, VBOBuffer, vertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT); //Creates a vertex buffer on GPU
		vulkanRenderer->CreateBuffer<uint32_t>(indices, EBOBuffer, indexBufferMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT); // Creates Index buffer on GPU
		
		vulkanRenderer->CreateUniformBuffer(&UBOBuffer, &uniformBufferMemory); //Sends uniforms each frame to the GPU

		vbo = uint64_t(VBOBuffer);
		ebo = uint64_t(EBOBuffer);
		ubo = uint64_t(UBOBuffer);
	}

	void VulkanMesh::AddTexture(eastl::string textureName)
	{
		Texture* texture = ModelLoader::CreateTexture(textureName);
		if (texture == nullptr) return;

		VulkanTexture* vulkanTexture = dynamic_cast<VulkanTexture*>(texture);
		VkBuffer ubo = GetUBO<VkBuffer>();
		vulkanTexture->CreateDescriptorSet(&ubo);
		textures.push_back(vulkanTexture);
	}
} //namespace Engine