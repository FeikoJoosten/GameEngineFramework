#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN

#include "Engine/Material/Material.hpp"

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Renderer/Vulkan/VulkanDescriptorPool.hpp"
#include "Engine/Renderer/Vulkan/VulkanBuffer.hpp"
#include "Engine/Texture/VulkanTexture.hpp"

#include <vector>

namespace Engine {

	class VulkanRenderer;

	class VulkanMaterial : public Material
	{
	public:

		/// <summary>
		/// Creates a descriptor set layout for the given device.
		/// </summary>
		/// <param name="device">The logical device to create the layout for.</param>
		/// <returns>A VkDescriptorSetLayout object.</returns>
		static VkDescriptorSetLayout CreateMaterialDescriptorSetLayout(VulkanLogicalDevice* device);

		VulkanMaterial(const aiScene* scene, uint32_t materialIndex, std::string modelName);
		~VulkanMaterial();

		/// <summary>
		/// Creates a new discriptor set bound to the mesh's material buffer, and configured for the given pipeline and layout.
		/// </summary>
		/// <param name="pipelineID">The pipeline the discriptor will be bound to.</param>
		/// <param name="set">The descriptor set number this descriptor will be bound to.</param>
		/// <param name="layout">The layout of the descriptor set.</param>
		/// <returns></returns>
		VkDescriptorSet CreateMaterialDescriptorSet(size_t threadID, size_t pipelineID, size_t set, VkDescriptorSetLayout layout);

		/// <summary>
		/// Returns the material descriptor set bound to the specified pipeline and set number.
		/// </summary>
		/// <param name="pipelineID">The pipeline this descriptor is bound to.</param>
		/// <param name="set">The set of the pipeline this descriptor is bound to.</param>
		/// <returns>The descriptor set bound to the pipeline and set number. 
		/// Returns VK_NULL_HANDLE of no descriptor is bound to the given combination.</returns>
		VkDescriptorSet GetMaterialDescriptorSet(size_t threadID, size_t pipelineID, size_t set);

		/// <summary>
		/// Changes the diffuse texture of the material. Pass a nullptr to clear the current texture.
		/// </summary>
		/// <param name="diffuseTexture">The new diffuse texture.</param>
		virtual void SetDiffuseTexture(std::shared_ptr<Texture> diffuseTexture);

		/// <summary>
		/// Changes the bump map texture of the material. Pass a nullptr to clear the current texture.
		/// </summary>
		/// <param name="diffuseTexture">The new bump map texture.</param>
		virtual void SetBumpMapTexture(std::shared_ptr<Texture> bumpMapTexture);
		
		/// <summary>
		/// Changes the specular texture of the material. Pass a nullptr to clear the current texture.
		/// </summary>
		/// <param name="specularTexture">The new specular texture.</param>
		virtual void SetSpecularTexture(std::shared_ptr<Texture> specularTexture);

	protected:
		std::unique_ptr<VulkanBuffer> materialDataBuffer_;

		std::vector<std::vector<std::vector<VkDescriptorSet>>> materialDescriptorSets_;

		virtual void UpdateMaterialData();

		friend VulkanRenderer;

		static VulkanRenderer* renderer_;
		static VulkanLogicalDevice* device_;
		static VkCommandPool commandPool_;
		static VmaAllocator allocator_;
		static VulkanDescriptorPool* descriptorPool_;

		static void InitMaterialData(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool, VmaAllocator allocator);
	};

} // namespace Engine
#endif // USING_VULKAN