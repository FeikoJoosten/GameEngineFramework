#pragma once

#include "Engine/Utility/Defines.hpp"

#ifdef USING_VULKAN

#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>
#include <ThirdParty/EASTL-master/include/EASTL/string.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine {
	class VulkanLogicalDevice;
	class VulkanRenderer;

	class VulkanComputePipeline
	{
	public:
		typedef size_t descriptorSetHandle;

		VulkanComputePipeline(VulkanLogicalDevice* logicalDevice);
		~VulkanComputePipeline();

		void SetComputeShader(eastl::string name);

		void AddSpecializationMapEntry(uint32_t constantID, eastl::vector<uint32_t> data);

		void AddDescriptorSetBinding(descriptorSetHandle set, uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags shaderStage, const VkSampler* immutableSamplers);

		descriptorSetHandle CreateDescriptorSet();
		descriptorSetHandle AddDescriptorSetLayout(VkDescriptorSetLayout layout);

		VkDescriptorSetLayout GetDescriptorSetLayout(descriptorSetHandle handle);

		void AddPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size);
		
		VkPipelineLayout GetPipelineLayout() const;
		
		void Compile();

		VkPipeline GetPipeline() const;

	protected:
		struct DescriptorSet {
			eastl::vector<VkDescriptorSetLayoutBinding> descriptionSetLayoutBindings;
			VkDescriptorSetLayout descriptorSet;
			bool external;
		};

		eastl::vector<char> computeShaderData_;

		VkShaderModule computeShader_;

		VkPipeline pipeline_;

		VkSpecializationInfo computeShaderSpecializationInfo_;

		eastl::vector<VkSpecializationMapEntry> specializationMapEntries_;
		eastl::vector<uint32_t> specializationData_;

		eastl::vector<DescriptorSet> descriptorSets_;

		eastl::vector<VkPushConstantRange> pushConstantRanges_;

		VkPipelineLayout pipelineLayout_;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo_;

		VulkanLogicalDevice * device_;
	};

} // namespace Engine

#endif // USING_VULKAN