#pragma once

#include "Engine/Utility/Defines.hpp"

#ifdef USING_VULKAN

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace Engine {
	class VulkanLogicalDevice;
	class VulkanRenderer;

	class VulkanComputePipeline
	{
	public:
		typedef size_t descriptorSetHandle;

		VulkanComputePipeline(VulkanLogicalDevice* logicalDevice);
		~VulkanComputePipeline();

		void SetComputeShader(std::string name);

		void AddSpecializationMapEntry(uint32_t constantID, std::vector<uint32_t> data);

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
			std::vector<VkDescriptorSetLayoutBinding> descriptionSetLayoutBindings;
			VkDescriptorSetLayout descriptorSet;
			bool external;
		};

		std::vector<char> computeShaderData_;

		VkShaderModule computeShader_;

		VkPipeline pipeline_;

		VkSpecializationInfo computeShaderSpecializationInfo_;

		std::vector<VkSpecializationMapEntry> specializationMapEntries_;
		std::vector<uint32_t> specializationData_;

		std::vector<DescriptorSet> descriptorSets_;

		std::vector<VkPushConstantRange> pushConstantRanges_;

		VkPipelineLayout pipelineLayout_;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo_;

		VulkanLogicalDevice * device_;
	};

}

#endif