#pragma once
#include "Engine/Mesh/Mesh.h"
#include "Engine/Texture/VulkanTexture.hpp"
#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>

namespace Engine
{
	class ENGINE_API VulkanMesh : public Mesh
	{
	public:
		VulkanMesh() = delete;
		VulkanMesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices, eastl::vector<Texture*> textures);
		VulkanMesh(VulkanMesh const &other) = default;
		~VulkanMesh() = default;
		void SetUpMesh() override;
		void AddTexture(eastl::string textureName) override;
		VkDeviceMemory getUniformMemory() { return uniformBufferMemory; }

	private:
		
		VkDeviceMemory vertexBufferMemory;

		VkDeviceMemory indexBufferMemory;
		VkDeviceMemory uniformBufferMemory;

	};

} //namespace Engine

