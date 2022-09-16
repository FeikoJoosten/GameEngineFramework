#include "Engine/Renderer/Vulkan/VulkanPipeline.hpp"

#ifdef USING_VULKAN
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"
#include <fstream>
#include <iostream>
#include <stdio.h>

namespace Engine {

	uint32_t VulkanPipeline::pipelineIdCounter = 0;

	VulkanPipeline::VulkanPipeline(VulkanLogicalDevice* device, VulkanRenderer* renderer) {
		this->pipelineId = pipelineIdCounter;
		pipelineIdCounter++;

		this->device = device;
		this->renderer = renderer;

		InputAssemblyStateCreateInfo = {};
		InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		viewport = {};
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float>(renderer->swapChainImageExtent.width);
		viewport.height = static_cast<float>(renderer->swapChainImageExtent.height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		scissor = {};
		scissor.offset = { 0,0 };
		scissor.extent = renderer->swapChainImageExtent;

		rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.f;
		rasterizer.depthBiasClamp = 0.f;
		rasterizer.depthBiasSlopeFactor = 0.f;

		multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilStateCreateInfo.minDepthBounds = 0.f;
		depthStencilStateCreateInfo.maxDepthBounds = 1.f;
		depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		depthStencilStateCreateInfo.front = {};
		depthStencilStateCreateInfo.back = {};


	}

	VulkanPipeline::~VulkanPipeline() {
		const VkDevice& vulkanDevice = device->GetDevice();
		if (pipeline != nullptr)
			vkDestroyPipeline(vulkanDevice, pipeline, nullptr);

		if (pipelineLayout != nullptr)
			vkDestroyPipelineLayout(vulkanDevice, pipelineLayout, nullptr);

		for (const DescriptorSet_t& descriptorSet : descriptorSets) {
			vkDestroyDescriptorSetLayout(vulkanDevice, descriptorSet.descriptorSet, nullptr);
		}

		if (descriptorSetLayout != nullptr)
			vkDestroyDescriptorSetLayout(vulkanDevice, descriptorSetLayout, nullptr);

		if (vertexShaderModule != nullptr)
			vkDestroyShaderModule(vulkanDevice, vertexShaderModule, nullptr);

		if (teslationEvaluationShaderModule != nullptr)
			vkDestroyShaderModule(vulkanDevice, teslationEvaluationShaderModule, nullptr);

		if (teslationControlShaderModule != nullptr)
			vkDestroyShaderModule(vulkanDevice, teslationControlShaderModule, nullptr);

		if (geometryShaderModule != nullptr)
			vkDestroyShaderModule(vulkanDevice, geometryShaderModule, nullptr);

		if (fragmentShaderModule != nullptr)
			vkDestroyShaderModule(vulkanDevice, fragmentShaderModule, nullptr);

	}

	bool VulkanPipeline::LoadShader(SHADER_TYPE type, std::string name, bool spirv) {
		if (!spirv)
			return false;

		std::string path = AssetManager::Get()->GetProjectRoot() + "Resources/Shaders/Vulkan/Compiled/" + name;

		FILE* file = fopen(path.c_str(), "rb");

		if (!file) {
			std::string s = "[ERROR] Opening shader file " + path + " failed";
			std::cout << s.c_str() << std::endl;
			return false;
		}
		fseek(file, 0L, SEEK_END);
		size_t fileSize = static_cast<size_t>(ftell(file));
		std::vector<char> buffer(fileSize);
		fseek(file, 0L, SEEK_SET);
		fread(buffer.data(), sizeof(char), fileSize, file);

		int ret;

		switch (type) {
		case SHADER_TYPE::VERTEX_SHADER:
			vertexShader = buffer;
			ret = CompileShaderModule(buffer, &vertexShaderModule);
			if (ret != 0) {
				vertexShader.clear();
			} else {
				VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
				vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertShaderStageInfo.module = vertexShaderModule;
				vertShaderStageInfo.pName = "main";
				ShaderStageCreateInfo[0] = vertShaderStageInfo;
			}

			break;
		case SHADER_TYPE::TESLATION_EVALUATION_SHADER:
			teslationEvaluationShader = buffer;
			ret = CompileShaderModule(buffer, &teslationEvaluationShaderModule);
			if (ret != 0)
				teslationEvaluationShader.clear();
			else {
				VkPipelineShaderStageCreateInfo tesEvalShaderStageInfo = {};
				tesEvalShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				tesEvalShaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
				tesEvalShaderStageInfo.module = teslationEvaluationShaderModule;
				tesEvalShaderStageInfo.pName = "main";
				ShaderStageCreateInfo[1] = tesEvalShaderStageInfo;
			}
			break;
		case SHADER_TYPE::TESLATION_CONTROL_SHADER:
			teslationControlShader = buffer;
			ret = CompileShaderModule(buffer, &teslationControlShaderModule);
			if (ret != 0)
				teslationControlShader.clear();
			else {
				VkPipelineShaderStageCreateInfo tesCtrlShaderStageInfo = {};
				tesCtrlShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				tesCtrlShaderStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				tesCtrlShaderStageInfo.module = teslationControlShaderModule;
				tesCtrlShaderStageInfo.pName = "main";
				ShaderStageCreateInfo[2] = tesCtrlShaderStageInfo;
			}
			break;
		case SHADER_TYPE::GEOMETRY_SHADER:
			geometryShader = buffer;
			ret = CompileShaderModule(buffer, &geometryShaderModule);
			if (ret != 0)
				geometryShader.clear();
			else {
				VkPipelineShaderStageCreateInfo geomShaderStageInfo = {};
				geomShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				geomShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				geomShaderStageInfo.module = geometryShaderModule;
				geomShaderStageInfo.pName = "main";
				ShaderStageCreateInfo[3] = geomShaderStageInfo;
			}
			break;
		case SHADER_TYPE::FRAGMENT_SHADER:
			fragmentShader = buffer;
			ret = CompileShaderModule(buffer, &fragmentShaderModule);
			if (ret != 0)
				fragmentShader.clear();
			else {
				VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
				fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragShaderStageInfo.module = fragmentShaderModule;
				fragShaderStageInfo.pName = "main";
				ShaderStageCreateInfo[4] = fragShaderStageInfo;
			}
			break;
		}

		fclose(file);
		if (ret != 0)
			return false;
		return true;
	}

	int VulkanPipeline::Compile() {
		if (vertexShader.size() == 0)
			return VULKAN_PIPELINE_COMPILE_MISSING_SHADER_VERTEX;
		if (fragmentShader.size() == 0)
			return VULKAN_PIPELINE_COMPILE_MISSING_SHADER_FRAGMENT;

		VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
		viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportCreateInfo.viewportCount = 1;
		viewportCreateInfo.pViewports = &viewport;
		viewportCreateInfo.scissorCount = 1;
		viewportCreateInfo.pScissors = &scissor;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		std::vector<VkDynamicState> dynamicStates;

		dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		std::vector<VkDescriptorSetLayout> descriptorLayouts;

		for (DescriptorSet_t& descriptorSet : descriptorSets) {
			if (descriptorSet.descriptorSet == nullptr) {
				VkDescriptorSetLayoutCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				createInfo.bindingCount = static_cast<uint32_t>(descriptorSet.descriptionSetLayoutBindings.size());
				createInfo.pBindings = descriptorSet.descriptionSetLayoutBindings.data();
				vkCreateDescriptorSetLayout(device->GetDevice(), &createInfo, nullptr, &descriptorSet.descriptorSet);
			}
			descriptorLayouts.push_back(descriptorSet.descriptorSet);
		}


		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

		VkResult res = vkCreatePipelineLayout(device->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
		if (res != VK_SUCCESS) {
			std::string s = std::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Creating pipeline layout failed";
			std::cout << s.c_str() << std::endl;
			return res;
		}

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		for (int i = 0; i < 5; i++) {
			if (ShaderStageCreateInfo[i].sType != 0) {
				shaderStages.push_back(ShaderStageCreateInfo[i]);
			}
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &InputAssemblyStateCreateInfo;
		pipelineInfo.pViewportState = &viewportCreateInfo;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencilStateCreateInfo;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = subPass;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		res = vkCreateGraphicsPipelines(device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
		if (res != VK_SUCCESS) {
			std::string s = std::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Creating pipeline failed";
			std::cout << s.c_str() << std::endl;
			return res;
		}

		return 0;
	}

	void VulkanPipeline::SetInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnabled) {
		InputAssemblyStateCreateInfo = {};
		InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyStateCreateInfo.topology = topology;
		if (primitiveRestartEnabled)
			InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_TRUE;
		else
			InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
	}

	void VulkanPipeline::AddVertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) {
		VkVertexInputBindingDescription description = {};
		description.binding = binding;
		description.stride = stride;
		description.inputRate = inputRate;
		vertexInputBindingDescriptions.push_back(description);

		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
	}

	void VulkanPipeline::AddVertexInputBindingDescription(VkVertexInputBindingDescription description) {
		vertexInputBindingDescriptions.push_back(description);

		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
	}

	void VulkanPipeline::AddVertexInputAttributeDescription(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) {
		VkVertexInputAttributeDescription description = {};
		description.location = location;
		description.binding = binding;
		description.format = format;
		description.offset = offset;
		vertexInputAttributeDescriptions.push_back(description);

		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
	}

	void VulkanPipeline::AddVertexInputAttributeDescription(VkVertexInputAttributeDescription description) {
		vertexInputAttributeDescriptions.push_back(description);

		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
	}

	void VulkanPipeline::SetRasterizerSettings(VkBool32 depthClamp, VkBool32 discardEnable, VkPolygonMode polygonMode, float lineWidth, VkCullModeFlagBits culling, VkFrontFace frontFace, VkBool32 depthBiasEnabled, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
		rasterizer.depthClampEnable = depthClamp;
		rasterizer.rasterizerDiscardEnable = discardEnable;
		rasterizer.polygonMode = polygonMode;
		rasterizer.lineWidth = lineWidth;
		rasterizer.cullMode = culling;
		rasterizer.frontFace = frontFace;
		rasterizer.depthBiasEnable = depthBiasEnabled;
		rasterizer.depthBiasConstantFactor = depthBiasConstantFactor;
		rasterizer.depthBiasClamp = depthBiasClamp;
		rasterizer.depthBiasSlopeFactor = depthBiasSlopeFactor;
	}

	void VulkanPipeline::CreateColorBlendAttachment(VkBool32 blendEnabled, VkColorComponentFlags colorWriteMask, VkBlendFactor colorSrcFactor, VkBlendFactor colorDstFactor, VkBlendOp colorBlendOp, VkBlendFactor alphaSrcFactor, VkBlendFactor alphaDstFactor, VkBlendOp alphaBlendOp) {
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		colorBlendAttachment.blendEnable = blendEnabled;
		colorBlendAttachment.colorWriteMask = colorWriteMask;
		colorBlendAttachment.srcColorBlendFactor = colorSrcFactor;
		colorBlendAttachment.dstColorBlendFactor = colorDstFactor;
		colorBlendAttachment.colorBlendOp = colorBlendOp;
		colorBlendAttachment.srcAlphaBlendFactor = alphaSrcFactor;
		colorBlendAttachment.dstAlphaBlendFactor = alphaDstFactor;
		colorBlendAttachment.alphaBlendOp = alphaBlendOp;

		colorBlendAttachments.push_back(colorBlendAttachment);
	}

	void VulkanPipeline::SetDepthStencilState(VkBool32 depthTestEnable,
		VkBool32 depthWriteEnable,
		VkCompareOp depthCompareOp,
		VkBool32 depthBoundsTestEnable,
		float minDepthBounds,
		float maxDepthBounds,
		VkBool32 stencilTestEnable,
		VkStencilOpState front,
		VkStencilOpState back) {
		depthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
		depthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
		depthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
		depthStencilStateCreateInfo.depthBoundsTestEnable = depthBoundsTestEnable;
		depthStencilStateCreateInfo.minDepthBounds = minDepthBounds;
		depthStencilStateCreateInfo.maxDepthBounds = maxDepthBounds;
		depthStencilStateCreateInfo.stencilTestEnable = stencilTestEnable;
		depthStencilStateCreateInfo.front = front;
		depthStencilStateCreateInfo.back = back;
	}

	void VulkanPipeline::AddDescriptorSetBinding(descriptorSetHandle set, uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags shaderStage, const VkSampler* immutableSamplers) {
		VkDescriptorSetLayoutBinding descriptorBinding = {};
		descriptorBinding.binding = binding;
		descriptorBinding.descriptorCount = descriptorCount;
		descriptorBinding.descriptorType = type;
		descriptorBinding.stageFlags = shaderStage;
		descriptorBinding.pImmutableSamplers = immutableSamplers;

		if (set < descriptorSets.size()) {
			descriptorSets[set].descriptionSetLayoutBindings.push_back(descriptorBinding);
		}
	}

	void VulkanPipeline::SetRenderPassInfo(VkRenderPass renderPass, uint32_t subPass) {
		this->renderPass = renderPass;
		this->subPass = subPass;
	}

	VulkanPipeline::descriptorSetHandle VulkanPipeline::CreateDescriptorSet() {
		DescriptorSet_t set = {};
		descriptorSets.push_back(set);
		return descriptorSets.size() - 1;
	}

	VulkanPipeline::descriptorSetHandle VulkanPipeline::AddDescriptorSetLayout(VkDescriptorSetLayout layout) {
		DescriptorSet_t set = {};
		set.descriptorSet = layout;
		set.external = true;
		descriptorSets.push_back(set);
		return descriptorSets.size() - 1;
	}

	VkDescriptorSetLayout VulkanPipeline::GetDescriptorSetLayout(descriptorSetHandle handle) {
		return descriptorSets[handle].descriptorSet;
	}

	VkPipeline VulkanPipeline::GetPipeline() const {
		return pipeline;
	}

	VkPipelineLayout VulkanPipeline::GetPipelineLayout() const {
		return pipelineLayout;
	}

	uint32_t VulkanPipeline::GetPipelineId() const {
		return pipelineId;
	}

	void VulkanPipeline::AddPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size) {
		VkPushConstantRange range = {};
		range.stageFlags = stageFlags;
		range.size = size;
		range.offset = offset;

		pushConstantRanges.push_back(range);
	}

	void VulkanPipeline::Clean() const {
		vkDestroyPipeline(device->GetDevice(), pipeline, nullptr);
		vkDestroyPipelineLayout(device->GetDevice(), pipelineLayout, nullptr);
	}

	void VulkanPipeline::Recreate() {
		viewport = {};
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float>(renderer->swapChainImageExtent.width);
		viewport.height = static_cast<float>(renderer->swapChainImageExtent.height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		scissor = {};
		scissor.offset = { 0,0 };
		scissor.extent = renderer->swapChainImageExtent;

		VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
		viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportCreateInfo.viewportCount = 1;
		viewportCreateInfo.pViewports = &viewport;
		viewportCreateInfo.scissorCount = 1;
		viewportCreateInfo.pScissors = &scissor;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		std::vector<VkDynamicState> states;

		states.push_back(VK_DYNAMIC_STATE_SCISSOR);

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(states.size());
		dynamicState.pDynamicStates = states.data();

		std::vector<VkDescriptorSetLayout> descriptorLayouts;

		for (DescriptorSet_t& descriptorSet : descriptorSets) {
			if (descriptorSet.descriptorSet == nullptr) {
				VkDescriptorSetLayoutCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				createInfo.bindingCount = static_cast<uint32_t>(descriptorSet.descriptionSetLayoutBindings.size());
				createInfo.pBindings = descriptorSet.descriptionSetLayoutBindings.data();
				vkCreateDescriptorSetLayout(device->GetDevice(), &createInfo, nullptr, &descriptorSet.descriptorSet);
			}
			descriptorLayouts.push_back(descriptorSet.descriptorSet);

		}

		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorLayouts.data();

		VkResult res = vkCreatePipelineLayout(device->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
		if (res != VK_SUCCESS) {
			std::string s = std::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Creating pipeline layout failed";
			std::cout << s.c_str() << std::endl;
		}

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		for (int i = 0; i < 5; i++) {
			if (ShaderStageCreateInfo[i].sType != 0) {
				shaderStages.push_back(ShaderStageCreateInfo[i]);
			}
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &InputAssemblyStateCreateInfo;
		pipelineInfo.pViewportState = &viewportCreateInfo;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencilStateCreateInfo;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = subPass;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		res = vkCreateGraphicsPipelines(device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
		if (res != VK_SUCCESS) {
			std::string s = std::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Creating pipeline failed";
			std::cout << s.c_str() << std::endl;
		}

	}

	int VulkanPipeline::CompileShaderModule(std::vector<char> code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkResult res = vkCreateShaderModule(device->GetDevice(), &createInfo, nullptr, shaderModule);
		if (res != VK_SUCCESS) {
			std::string s = std::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Creating shader module failed";
			std::cout << s.c_str() << std::endl;
		}

		return res;
	}
}

#endif // USING_VULKAN