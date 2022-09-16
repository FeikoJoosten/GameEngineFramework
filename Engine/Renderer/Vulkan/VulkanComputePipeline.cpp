#include "Engine/Renderer/Vulkan/VulkanComputePipeline.hpp"
#ifdef USING_VULKAN

#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"

#include "Engine/Utility/Logging.hpp"

#include <stdio.h>

namespace Engine {
	
	VulkanComputePipeline::VulkanComputePipeline(VulkanLogicalDevice * logicalDevice)
	{
		this->device_ = logicalDevice;
	}

	VulkanComputePipeline::~VulkanComputePipeline()
	{
		vkDestroyPipeline(device_->GetDevice(), pipeline_, nullptr);

		vkDestroyPipelineLayout(device_->GetDevice(), pipelineLayout_, nullptr);

		for (size_t i = 0, size = descriptorSets_.size(); i < size; ++i) {
			vkDestroyDescriptorSetLayout(device_->GetDevice(), descriptorSets_[i].descriptorSet, nullptr);
		}

		vkDestroyShaderModule(device_->GetDevice(), computeShader_, nullptr);
	}

	void VulkanComputePipeline::SetComputeShader(std::string name)
	{
		std::string path = AssetManager::Get()->GetProjectRoot() + "Resources/Shaders/Vulkan/Compiled/" + name;

		FILE* file = fopen(path.c_str(), "rb");
		if (!file) {
			std::string s = "Opening shader file " + path + " failed";
			debug_error("VulkanComputePipeline", "SetComputeShader", s);
			return;
		}

		fseek(file, 0L, SEEK_END);
		size_t fileSize = static_cast<size_t>(ftell(file));
		computeShaderData_.resize(fileSize);
		fseek(file, 0L, SEEK_SET);
		fread(computeShaderData_.data(), sizeof(char), fileSize, file);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = computeShaderData_.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(computeShaderData_.data());

		vkCreateShaderModule(device_->GetDevice(), &createInfo, nullptr, &computeShader_);
	}

	void VulkanComputePipeline::AddSpecializationMapEntry(uint32_t constantID, std::vector<uint32_t> data)
	{
		VkSpecializationMapEntry entry = {};
		entry.constantID = constantID;
		entry.offset = static_cast<uint32_t>(sizeof(uint32_t)*specializationData_.size());
		entry.size = static_cast<uint32_t>(sizeof(uint32_t)*data.size());

		specializationData_.insert(specializationData_.end(), data.begin(), data.end());
	}

	void VulkanComputePipeline::AddDescriptorSetBinding(descriptorSetHandle set, uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags shaderStage, const VkSampler * immutableSamplers)
	{
		VkDescriptorSetLayoutBinding descriptorBinding = {};
		descriptorBinding.binding = binding;
		descriptorBinding.descriptorCount = descriptorCount;
		descriptorBinding.descriptorType = type;
		descriptorBinding.stageFlags = shaderStage;
		descriptorBinding.pImmutableSamplers = immutableSamplers;

		if (set<descriptorSets_.size()) {
			descriptorSets_[set].descriptionSetLayoutBindings.push_back(descriptorBinding);
		}
	}

	VulkanComputePipeline::descriptorSetHandle VulkanComputePipeline::CreateDescriptorSet()
	{
		DescriptorSet set = {};
		descriptorSets_.push_back(set);
		return descriptorSets_.size() - 1;
	}

	VulkanComputePipeline::descriptorSetHandle VulkanComputePipeline::AddDescriptorSetLayout(VkDescriptorSetLayout layout)
	{
		DescriptorSet set = {};
		set.descriptorSet = layout;
		set.external = true;
		descriptorSets_.push_back(set);
		return descriptorSets_.size() - 1;
	}

	VkDescriptorSetLayout VulkanComputePipeline::GetDescriptorSetLayout(descriptorSetHandle handle)
	{
		return descriptorSets_[handle].descriptorSet;
	}

	void VulkanComputePipeline::AddPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size)
	{
		VkPushConstantRange range = {};
		range.stageFlags = stageFlags;
		range.size = size;
		range.offset = offset;

		pushConstantRanges_.push_back(range);
	}

	VkPipelineLayout VulkanComputePipeline::GetPipelineLayout() const
	{
		return pipelineLayout_;
	}

	void VulkanComputePipeline::Compile()
	{
		computeShaderSpecializationInfo_ = {};
		computeShaderSpecializationInfo_.mapEntryCount = static_cast<uint32_t>(specializationMapEntries_.size());
		computeShaderSpecializationInfo_.pMapEntries = specializationMapEntries_.data();
		computeShaderSpecializationInfo_.dataSize = static_cast<uint32_t>(specializationData_.size() * sizeof(uint32_t));
		computeShaderSpecializationInfo_.pData = specializationData_.data();

		VkPipelineShaderStageCreateInfo shaderInfo = {};
		shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderInfo.module = computeShader_;
		shaderInfo.pName = "main";
		shaderInfo.pSpecializationInfo = &computeShaderSpecializationInfo_;

		std::vector<VkDescriptorSetLayout> descriptorLayouts;

		for (size_t i = 0, size = descriptorSets_.size(); i < size; ++i) {
			if (descriptorSets_[i].descriptorSet == 0) {
				VkDescriptorSetLayoutCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				createInfo.bindingCount = static_cast<uint32_t>(descriptorSets_[i].descriptionSetLayoutBindings.size());
				createInfo.pBindings = descriptorSets_[i].descriptionSetLayoutBindings.data();

				vkCreateDescriptorSetLayout(device_->GetDevice(), &createInfo, nullptr, &descriptorSets_[i].descriptorSet);
			}
			descriptorLayouts.push_back(descriptorSets_[i].descriptorSet);
		}

		pipelineLayoutInfo_ = {};
		pipelineLayoutInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo_.pPushConstantRanges = pushConstantRanges_.data();
		pipelineLayoutInfo_.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges_.size());
		pipelineLayoutInfo_.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
		pipelineLayoutInfo_.pSetLayouts = descriptorLayouts.data();

		vkCreatePipelineLayout(device_->GetDevice(), &pipelineLayoutInfo_, nullptr, &pipelineLayout_);

		VkComputePipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.stage = shaderInfo;
		pipelineInfo.layout = pipelineLayout_;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		vkCreateComputePipelines(device_->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline_);
	}

	VkPipeline VulkanComputePipeline::GetPipeline() const
	{
		return pipeline_;
	}

} // namespace Engine

#endif // USING_VULKAN
