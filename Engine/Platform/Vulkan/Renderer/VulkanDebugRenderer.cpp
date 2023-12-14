#include "Engine/Platform/Vulkan/Renderer/VulkanDebugRenderer.hpp"
#ifdef USING_VULKAN

#include "Engine/Platform/Vulkan/Renderer/VulkanRenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	VulkanDebugRenderer::VulkanDebugRenderer(VulkanRenderer * renderer, VulkanLogicalDevice * device, VulkanDescriptorPool * descriptorPool)
	{
		this->renderer = renderer;
		this->device = device;
		this->descriptorPool = descriptorPool;

		linePipeline = std::unique_ptr<VulkanPipeline>(new VulkanPipeline(device, renderer));

		linePipeline->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "Line.vert.spv");
		linePipeline->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "Line.frag.spv");

		linePipeline->AddVertexInputBindingDescription(0, static_cast<uint32_t>(sizeof(VertexInfo_t)), VK_VERTEX_INPUT_RATE_VERTEX);
		linePipeline->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);

		linePipeline->CreateDescriptorSet();

		linePipeline->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr);

		linePipeline->AddPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, static_cast<uint32_t>(sizeof(PushConstants_t)));

		linePipeline->SetRenderPassInfo(renderer->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS));

		linePipeline->CreateColorBlendAttachment();

		linePipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false);

		linePipeline->SetRasterizerSettings();

		linePipeline->Compile();

		vertexBuffer = std::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, renderer->GetVmaAllocator(),
			static_cast<uint32_t>(sizeof(VertexInfo_t) * 2), 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, true, renderer->GetGraphicsCommandPool()));

		uniformBuffer = std::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, renderer->GetVmaAllocator(),
			static_cast<uint32_t>(sizeof(Ubo_t)),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, false, renderer->GetGraphicsCommandPool()));

		std::vector<VertexInfo_t> vertexData = {
			{{0.f,0.f,0.f}},
			{{0.f,1.f,0.f}}
		};

		vertexBuffer->UpdateBuffer(vertexData.data(), 0, static_cast<uint32_t>(sizeof(VertexInfo_t)*vertexData.size()));

		ubo = { glm::mat4(), glm::mat4() };

		uniformBuffer->UpdateBuffer(&ubo, 0, static_cast<uint32_t>(sizeof(ubo)));

		VkDescriptorSetLayout layouts[] = { linePipeline->GetDescriptorSetLayout(0) };
		descriptorPool->AllocateDescriptorSet(1, layouts, &uboDescriptorSet);

		descriptorPool->DescriptorSetBindToBuffer(uboDescriptorSet, uniformBuffer->GetBuffer(), 0, static_cast<VkDeviceSize>(sizeof(ubo)),
			0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);


	}

	VulkanDebugRenderer::~VulkanDebugRenderer()
	{
		vertexBuffer.reset();
		uniformBuffer.reset();

		linePipeline.reset();
	}

	void VulkanDebugRenderer::StartRender(VkCommandBuffer commandBuffer, glm::mat4 view, glm::mat4 projection)
	{
		this->commandBuffer = commandBuffer;

		if (ubo.view != view || ubo.proj != projection) {
			ubo.view = view;
			ubo.proj = projection;

			uniformBuffer->UpdateBuffer(&ubo, 0, static_cast<uint32_t>(sizeof(ubo)));
		}

		renderer->StartSecondaryCommandBufferRecording(commandBuffer,
			VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			renderer->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS), renderer->GetFrameBuffer());

		VkRect2D scissor = { 0,0,renderer->GetSwapChainExtent() };

		VkBuffer vertexBuffers[] = { (vertexBuffer->GetBuffer()) };
		VkDeviceSize offsets[] = { 0 };

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, linePipeline->GetPipeline());

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, linePipeline->GetPipelineLayout(), 0, 1, &uboDescriptorSet, 0, nullptr);

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		
	}

	void VulkanDebugRenderer::RenderLine(glm::vec3 start, glm::vec3 end, glm::vec4 color)
	{
		PushConstants_t constants = {};

		glm::vec3 translation = start;

		glm::vec3 rotationVector = glm::cross(glm::vec3(0.f, 1.f, 0.f), glm::normalize(end - start));
		float rotationAngle = acosf(glm::dot(glm::vec3(0.f, 1.f, 0.f), (end - start)) / (glm::length(end - start)));

		if (rotationVector == glm::vec3(0.f, 0.f, 0.f)) {
			rotationVector = glm::vec3(1.f, 1.f, 1.f);
			rotationAngle = 0.f;
		}

		float scale = glm::length(end - start);

		constants.model = glm::translate(glm::mat4(), translation)*
			glm::rotate(glm::mat4(), rotationAngle, rotationVector)*
			glm::scale(glm::mat4(), glm::vec3(1.f, scale, 1.f));
		constants.color = color;

		lines.push_back(constants);

	}

	void VulkanDebugRenderer::FinishRender()
	{
		for (size_t i = 0, size = lines.size(); i < size; ++i) {
			vkCmdPushConstants(commandBuffer, linePipeline->GetPipelineLayout(),
				VK_SHADER_STAGE_VERTEX_BIT, 0, static_cast<uint32_t>(sizeof(lines[i])), &lines[i]);

			vkCmdDraw(commandBuffer, 2, 1, 0, 0);
		}
		renderer->EndSecondaryCommandBufferRecording(commandBuffer);

		lines.clear();
	}

	void VulkanDebugRenderer::Clean()
	{
		linePipeline->Clean();
	}

	void VulkanDebugRenderer::Recreate()
	{
		linePipeline->SetRenderPassInfo(renderer->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS));
		linePipeline->Recreate();
	}

}

#endif