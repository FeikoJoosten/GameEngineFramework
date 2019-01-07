#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN

#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>
#include <ThirdParty/EASTL-master/include/EASTL/string.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

#define VULKAN_PIPELINE_COMPILE_MISSING_SHADER_VERTEX 1
#define VULKAN_PIPELINE_COMPILE_MISSING_SHADER_FRAGMENT 2

namespace Engine {
	class VulkanLogicalDevice;
	class VulkanRenderer;

	class VulkanPipeline {
	public:
		typedef size_t descriptorSetHandle;

		VulkanPipeline(VulkanLogicalDevice* device, VulkanRenderer* renderer);
		~VulkanPipeline();

		enum SHADER_TYPE {
			VERTEX_SHADER = 0,
			TESLATION_EVALUATION_SHADER,
			TESLATION_CONTROL_SHADER,
			GEOMETRY_SHADER,
			FRAGMENT_SHADER
		};

		/*	The system automatically looks in the Shaders\Compiled folder for shaders. Currently spirv need to be true. If it's false, the function fails.
		*	Returns false if the function fails
		*/
		bool LoadShader(SHADER_TYPE type, eastl::string name, bool spirv = true);

		/*	Returns 0 if succesfull, otherwise, an error code is returned.
		*/
		int Compile();

		void SetInputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnabled);

		void AddVertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
		void AddVertexInputBindingDescription(VkVertexInputBindingDescription description);
		void AddVertexInputAttributeDescription(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);
		void AddVertexInputAttributeDescription(VkVertexInputAttributeDescription description);
		void SetRasterizerSettings(VkBool32 depthClamp = VK_FALSE, VkBool32 discardEnable = VK_FALSE, VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL, float lineWidth = 1.f,
			VkCullModeFlagBits culling = VK_CULL_MODE_BACK_BIT, VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			VkBool32 depthBiasEnabled = VK_FALSE, float depthBiasConstantFactor = 0.f, float depthBiasClamp = 0.f, float depthBiasSlopeFactor = 0.f);

		void CreateColorBlendAttachment(VkBool32 blendEnabled = VK_TRUE, VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			VkBlendFactor colorSrcFactor = VK_BLEND_FACTOR_SRC_ALPHA, VkBlendFactor colorDstFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
			VkBlendFactor alphaSrcFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor alphaDstFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD);

		void SetDepthStencilState(VkBool32 depthTestEnable = VK_TRUE,
			VkBool32 depthWriteEnable = VK_TRUE,
			VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS,
			VkBool32 depthBoundsTestEnable = VK_FALSE,
			float minDepthBounds = 0.f,
			float maxDepthBounds = 1.f,
			VkBool32 stencilTestEnable = VK_FALSE,
			VkStencilOpState front = {},
			VkStencilOpState back = {});

		void AddDescriptorSetBinding(descriptorSetHandle set, uint32_t binding, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags shaderStage, const VkSampler* immutableSamplers);

		void SetRenderPassInfo(VkRenderPass renderPass, uint32_t subPass);

		descriptorSetHandle CreateDescriptorSet();
		descriptorSetHandle AddDescriptorSetLayout(VkDescriptorSetLayout layout);

		VkDescriptorSetLayout GetDescriptorSetLayout(descriptorSetHandle handle);

		VkPipeline GetPipeline() const;

		VkPipelineLayout GetPipelineLayout() const;

		uint32_t GetPipelineId() const;

		void AddPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size);

		void Clean() const;
		void Recreate();

	private:
		typedef struct {
			eastl::vector<VkDescriptorSetLayoutBinding> descriptionSetLayoutBindings;
			VkDescriptorSetLayout descriptorSet;
			bool external;
		}DescriptorSet_t;

		eastl::vector<char> vertexShader;
		eastl::vector<char> teslationEvaluationShader;
		eastl::vector<char> teslationControlShader;
		eastl::vector<char> geometryShader;
		eastl::vector<char> fragmentShader;

		VkShaderModule vertexShaderModule;
		VkShaderModule teslationEvaluationShaderModule;
		VkShaderModule teslationControlShaderModule;
		VkShaderModule geometryShaderModule;
		VkShaderModule fragmentShaderModule;

		VulkanLogicalDevice* device;
		VulkanRenderer* renderer;

		VkViewport viewport;
		VkRect2D scissor;

		VkPipelineLayout pipelineLayout;

		VkPipeline pipeline;

		VkDescriptorSetLayout descriptorSetLayout;

		VkPipelineRasterizationStateCreateInfo rasterizer;

		VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;

		VkPipelineMultisampleStateCreateInfo multisampling;
		eastl::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;

		VkRenderPass renderPass;
		uint32_t subPass;

		eastl::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
		eastl::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

		eastl::vector<DescriptorSet_t> descriptorSets;

		eastl::vector<VkPushConstantRange> pushConstantRanges;

		int CompileShaderModule(eastl::vector<char> code, VkShaderModule* shaderModule);

		VkPipelineShaderStageCreateInfo ShaderStageCreateInfo[5] = { {},{},{},{},{} };

		VkDynamicState dynamicStates[5] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR };

		uint32_t pipelineId;

		static uint32_t pipelineIdCounter;

	};

}

#endif // USING_VULKAN