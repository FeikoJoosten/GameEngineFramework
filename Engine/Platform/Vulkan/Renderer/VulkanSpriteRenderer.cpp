#include "Engine/Platform/Vulkan/Renderer/VulkanSpriteRenderer.hpp"
#ifdef USING_VULKAN
#include "Engine/Platform/Vulkan/Renderer/VulkanRenderer.hpp"

namespace Engine {
	
	VulkanSpriteRenderer::VulkanSpriteRenderer(VulkanRenderer * renderer, VulkanLogicalDevice * device, VulkanDescriptorPool * descriptorPool)
	{
		this->renderer = renderer;
		this->device = device;
		this->descriptorPool = descriptorPool;

		renderPassPipeline = std::unique_ptr<VulkanPipeline>(new VulkanPipeline(device, renderer));

		renderPassPipeline->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "Sprite.vert.spv");
		renderPassPipeline->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "Sprite.frag.spv");

		renderPassPipeline->AddVertexInputBindingDescription(0, static_cast<uint32_t>(sizeof(VertexInfo_t)), VK_VERTEX_INPUT_RATE_VERTEX);
		renderPassPipeline->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
		renderPassPipeline->AddVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexInfo_t, texCoord));

		renderPassPipeline->CreateDescriptorSet();
		renderPassPipeline->CreateDescriptorSet();

		renderPassPipeline->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr);

		renderPassPipeline->AddDescriptorSetBinding(1, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);

		renderPassPipeline->AddPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, static_cast<uint32_t>(sizeof(PushConstants_t)));

		renderPassPipeline->SetRenderPassInfo(renderer->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS));

		renderPassPipeline->CreateColorBlendAttachment(VK_TRUE, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);

		renderPassPipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);

		renderPassPipeline->SetRasterizerSettings(0U, 0U, VK_POLYGON_MODE_FILL, 1.f, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);

		renderPassPipeline->Compile();



		vertexBuffer = std::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, renderer->GetVmaAllocator(),
			static_cast<uint32_t>(sizeof(VertexInfo_t))*4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			true, renderer->GetGraphicsCommandPool()));

		indexBuffer = std::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, renderer->GetVmaAllocator(),
			static_cast<uint32_t>(sizeof(uint32_t)) * 6, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
			true, renderer->GetGraphicsCommandPool()));

		uboBuffer = std::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, renderer->GetVmaAllocator(),
			static_cast<uint32_t>(sizeof(Ubo_t)), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			false, renderer->GetGraphicsCommandPool()));

		ubo.proj = glm::mat4();
		ubo.view = glm::scale(glm::mat4(),glm::vec3(0.f,0.f,0.f));

		std::vector<VertexInfo_t> vertexData = {
			{glm::vec3(-0.5f,0.5f,0.0f),glm::vec2(0.f,1.f)},
			{glm::vec3(-0.5f,-0.5f,0.0f),glm::vec2(0.f,0.f)},
			{glm::vec3(0.5f,-0.5f,0.0f),glm::vec2(1.f,0.f)},
			{glm::vec3(0.5f,0.5f,0.0f),glm::vec2(1.f,1.f)}
		};

		indices = { 0,1,2,2,3,0 };

		vertexBuffer->UpdateBuffer(vertexData.data(), 0, static_cast<uint32_t>(sizeof(VertexInfo_t)*vertexData.size()));
		indexBuffer->UpdateBuffer(indices.data(), 0, static_cast<uint32_t>(sizeof(uint32_t)*indices.size()));
		uboBuffer->UpdateBuffer(&ubo, 0, static_cast<uint32_t>(sizeof(Ubo_t)));
		
		VkDescriptorSetLayout renderLayouts[] = { renderPassPipeline->GetDescriptorSetLayout(0) };

		descriptorPool->AllocateDescriptorSet(1, renderLayouts, &renderUboDescriptorSet);
		descriptorPool->DescriptorSetBindToBuffer(renderUboDescriptorSet, uboBuffer->GetBuffer(),
			0, static_cast<VkDeviceSize>(sizeof(Ubo_t)),
			0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
	}

	VulkanSpriteRenderer::~VulkanSpriteRenderer()
	{
		vertexBuffer.reset();
		indexBuffer.reset();
		uboBuffer.reset();

		renderPassPipeline.reset();
	}

	void VulkanSpriteRenderer::StartRender(VkCommandBuffer renderPassBuffer, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
	{
		this->renderCommandBuffer = renderPassBuffer;

		if (ubo.view != viewMatrix || ubo.proj != projectionMatrix) {
			ubo.view = viewMatrix;
			ubo.proj = projectionMatrix;

			uboBuffer->UpdateBuffer(&ubo, 0, static_cast<uint32_t>(sizeof(Ubo_t)));
		}

		VkRect2D scissor = { 0,0,renderer->GetSwapChainExtent() };

		VkBuffer vertexBuffers[] = { (vertexBuffer->GetBuffer()) };
		VkDeviceSize offsets[] = { 0 };

		renderer->StartSecondaryCommandBufferRecording(renderCommandBuffer, 
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			renderer->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS), renderer->GetFrameBuffer());

		vkCmdSetScissor(renderCommandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPassPipeline->GetPipeline());

		vkCmdBindDescriptorSets(renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPassPipeline->GetPipelineLayout(), 0, 1, &renderUboDescriptorSet, 0, nullptr);

		vkCmdBindVertexBuffers(renderCommandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(renderCommandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanSpriteRenderer::RenderSprite(std::weak_ptr<VulkanTexture> texture, glm::mat4 modelMatrix)
	{
		PushConstants_t constants = { modelMatrix };

		vkCmdPushConstants(renderCommandBuffer, renderPassPipeline->GetPipelineLayout(),
			VK_SHADER_STAGE_VERTEX_BIT, 0, static_cast<uint32_t>(sizeof(PushConstants_t)), &constants);

		if (texture.lock()->GetDescriptorSet(0, renderPassPipeline->GetPipelineId(), 1) == VK_NULL_HANDLE) {
			texture.lock()->CreateDescriptorSet(0, renderPassPipeline->GetPipelineId(), 1, renderPassPipeline->GetDescriptorSetLayout(1));
		}

		VkDescriptorSet sets[] = { texture.lock()->GetDescriptorSet(0, renderPassPipeline->GetPipelineId(),1) };
		vkCmdBindDescriptorSets(renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			renderPassPipeline->GetPipelineLayout(), 1, 1, sets, 0, nullptr);

		vkCmdDrawIndexed(renderCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	}

	void VulkanSpriteRenderer::FinishRender()
	{
		renderer->EndSecondaryCommandBufferRecording(renderCommandBuffer);
	}

	void VulkanSpriteRenderer::Clean() const
	{
		renderPassPipeline->Clean();
	}

	void VulkanSpriteRenderer::Recreate()
	{
		renderPassPipeline->SetRenderPassInfo(renderer->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS));
		renderPassPipeline->Recreate();
	}

}

#endif // USING_VULKAN