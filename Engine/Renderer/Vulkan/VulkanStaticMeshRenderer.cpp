#include "VulkanStaticMeshRenderer.hpp"

#ifdef USING_VULKAN

#include "Engine/Renderer/VulkanRenderer.hpp"
#include "Engine/Mesh/VulkanMesh.hpp"
#include "Engine/Material/VulkanMaterial.hpp"
#include "Engine/engine.hpp"

namespace Engine {
	
	VulkanStaticMeshRenderer::VulkanStaticMeshRenderer(VulkanRenderer * renderer, VulkanLogicalDevice * device, VulkanDescriptorPool * descriptorPool)
	{
		this->renderer_ = renderer;
		this->device_ = device;
		this->descriptorPool_ = descriptorPool;
		this->allocator_ = renderer->GetVmaAllocator();
		this->commandPool_ = renderer->GetGraphicsCommandPool();

		staticMeshPipeline_ = eastl::unique_ptr<VulkanPipeline>(new VulkanPipeline(device, renderer));

		staticMeshPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "StaticMesh.vert.spv");
		//staticMeshPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::GEOMETRY_SHADER, "Mesh.geom.spv");
		staticMeshPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "Mesh.frag.spv");

		staticMeshPipeline_->AddVertexInputBindingDescription(0, 
			static_cast<uint32_t>(sizeof(Vertex)), VK_VERTEX_INPUT_RATE_VERTEX);

		staticMeshPipeline_->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
		staticMeshPipeline_->AddVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
		staticMeshPipeline_->AddVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords));

		staticMeshPipeline_->AddVertexInputBindingDescription(1,
			static_cast<uint32_t>(sizeof(glm::mat4)), VK_VERTEX_INPUT_RATE_INSTANCE);

		staticMeshPipeline_->AddVertexInputAttributeDescription(3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(sizeof(glm::vec4) * 0));
		staticMeshPipeline_->AddVertexInputAttributeDescription(4, 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(sizeof(glm::vec4) * 1));
		staticMeshPipeline_->AddVertexInputAttributeDescription(5, 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(sizeof(glm::vec4) * 2));
		staticMeshPipeline_->AddVertexInputAttributeDescription(6, 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(sizeof(glm::vec4) * 3));

		staticMeshPipeline_->CreateDescriptorSet();
		staticMeshPipeline_->AddDescriptorSetLayout(VulkanMaterial::CreateMaterialDescriptorSetLayout(device));
		
		staticMeshPipeline_->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT, nullptr);
		
		staticMeshPipeline_->SetRenderPassInfo(renderer->GetGBufferRenderPass(), static_cast<int>(VulkanRenderer::GBufferSubPasses::G_BUFFER_PASS));

		staticMeshPipeline_->CreateColorBlendAttachment(VK_FALSE);
		staticMeshPipeline_->CreateColorBlendAttachment(VK_FALSE);
		staticMeshPipeline_->CreateColorBlendAttachment(VK_FALSE);

		staticMeshPipeline_->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);

		staticMeshPipeline_->SetRasterizerSettings();

		staticMeshPipeline_->Compile();

		shadowPipeline_ = eastl::unique_ptr<VulkanPipeline>(new VulkanPipeline(device_, renderer_));

		shadowPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "StaticShadow.vert.spv");
		shadowPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::GEOMETRY_SHADER, "ShadowVolume.geom.spv");
		shadowPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "ShadowVolume.frag.spv");

		shadowPipeline_->AddVertexInputBindingDescription(0,
			static_cast<uint32_t>(sizeof(Vertex)), VK_VERTEX_INPUT_RATE_VERTEX);

		shadowPipeline_->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
		shadowPipeline_->AddVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
		shadowPipeline_->AddVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords));

		shadowPipeline_->AddVertexInputBindingDescription(1,
			static_cast<uint32_t>(sizeof(glm::mat4)), VK_VERTEX_INPUT_RATE_INSTANCE);

		shadowPipeline_->AddVertexInputAttributeDescription(3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(sizeof(glm::vec4) * 0));
		shadowPipeline_->AddVertexInputAttributeDescription(4, 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(sizeof(glm::vec4) * 1));
		shadowPipeline_->AddVertexInputAttributeDescription(5, 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(sizeof(glm::vec4) * 2));
		shadowPipeline_->AddVertexInputAttributeDescription(6, 1, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(sizeof(glm::vec4) * 3));

		shadowPipeline_->CreateDescriptorSet();
		shadowPipeline_->AddDescriptorSetLayout(renderer_->CreateLightDescriptorSetLayout());

		shadowPipeline_->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT, nullptr);

		shadowPipeline_->SetRenderPassInfo(renderer->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS));

		shadowPipeline_->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY, false);

		shadowPipeline_->SetRasterizerSettings(VK_TRUE, VK_FALSE,
			VK_POLYGON_MODE_FILL, 1.f, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0, 0.f, 0.f);

		VkStencilOpState front = {};
		front.failOp = VK_STENCIL_OP_KEEP;
		front.passOp = VK_STENCIL_OP_KEEP;
		front.depthFailOp = VK_STENCIL_OP_DECREMENT_AND_WRAP;
		front.compareOp = VK_COMPARE_OP_ALWAYS;
		front.reference = 0;
		front.writeMask = 0xff;
		front.compareMask = 0xff;

		VkStencilOpState back = {};
		back.failOp = VK_STENCIL_OP_KEEP;
		back.passOp = VK_STENCIL_OP_KEEP;
		back.depthFailOp = VK_STENCIL_OP_INCREMENT_AND_WRAP;
		back.compareOp = VK_COMPARE_OP_ALWAYS;
		back.reference = 0;
		back.writeMask = 0xff;
		back.compareMask = 0xff;

		shadowPipeline_->SetDepthStencilState(VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS, VK_TRUE, 0.f, 1.f, VK_TRUE, front, back);

		shadowPipeline_->CreateColorBlendAttachment(VK_TRUE, 0u);

		shadowPipeline_->Compile();

		ubo_ = { glm::mat4(), glm::mat4() };

		uniformBuffer_ = eastl::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, renderer->GetVmaAllocator(),
			static_cast<uint32_t>(sizeof(ubo_)), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, false, renderer->GetGraphicsCommandPool()));

		uniformBuffer_->UpdateBuffer(&ubo_, 0, static_cast<uint32_t>(sizeof(ubo_)));

		uboDescriptors_.resize(renderer_->GetThreadCount());

		for (size_t i = 0, size = uboDescriptors_.size(); i < size; ++i) {

			VkDescriptorSetLayout layouts[] = { staticMeshPipeline_->GetDescriptorSetLayout(0) };
			renderer_->GetDescriptorPool(i)->AllocateDescriptorSet(1, layouts, &uboDescriptors_[i]);

			renderer_->GetDescriptorPool(i)->DescriptorSetBindToBuffer(uboDescriptors_[i], uniformBuffer_->GetBuffer(),
				0, static_cast<VkDeviceSize>(sizeof(ubo_)), 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
		
		}

		Engine::GetEngine().lock()->GetResourceManager().lock()->CreateTexture("default.png");
		defaultTexture_ = eastl::dynamic_pointer_cast<VulkanTexture, Texture>(
			Engine::GetEngine().lock()->GetResourceManager().lock()->GetTexture("default.png"));
	}

	VulkanStaticMeshRenderer::~VulkanStaticMeshRenderer()
	{

		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator it = meshInstances_.begin();
		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator end = meshInstances_.end();

		for (; it != end; ++it) {
			it->second->transformBuffer.reset();
			delete it->second;
		}

		uniformBuffer_.reset();
		
		staticMeshPipeline_.reset();
		shadowPipeline_.reset();
	}

	void VulkanStaticMeshRenderer::StartRender(glm::mat4 view, glm::mat4 projection)
	{
		if (ubo_.view != view || ubo_.proj != projection) {
			ubo_.view = view;
			ubo_.proj = projection;

			uniformBuffer_->UpdateBuffer(&ubo_, 0, static_cast<uint32_t>(sizeof(ubo_)));
		}
		
		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator it = meshInstances_.begin();
		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator end = meshInstances_.end();

		for (; it != end; ++it) {
			it->second->transforms.clear();
			it->second->meshes = 0;
		}
	}

	void VulkanStaticMeshRenderer::RenderMesh(const glm::mat4x4 & modelMatrix, eastl::shared_ptr<VulkanMesh> mesh,
		eastl::shared_ptr<VulkanMaterial> material, const glm::vec4 & mainColor)
	{
		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator it = meshInstances_.begin();
		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator end = meshInstances_.end();

		for (; it != end; ++it) {
			if (it->first == mesh && it->second->meshes < MAX_INSTANCE_COUNT && *(it->second->material)==*(material)) {
				it->second->transforms.push_back(modelMatrix);
				it->second->meshes++;
				break;
			}
		}

		if (it == end) {
			MeshData* data = new MeshData;
			data->material = material;
			data->transforms.push_back(modelMatrix);
			data->meshes = 1;
			data->transformBuffer = eastl::unique_ptr<VulkanBuffer>(new VulkanBuffer(
				device_, allocator_, static_cast<uint32_t>(sizeof(glm::mat4)*MAX_INSTANCE_COUNT),
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, false, commandPool_));
			meshInstances_.emplace(eastl::pair<eastl::shared_ptr<VulkanMesh>,MeshData*>(mesh, data));
		}
	}

	void VulkanStaticMeshRenderer::FinishRender(size_t threadID, VkCommandPool commandPool, VkCommandBuffer buffer)
	{
/*
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;*/

		VkCommandBuffer commandBuffer = buffer;

		//vkAllocateCommandBuffers(device->GetDevice(), &allocInfo, &commandBuffer);


		renderer_->StartSecondaryCommandBufferRecording(commandBuffer,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
			renderer_->GetGBufferRenderPass(), static_cast<int>(VulkanRenderer::GBufferSubPasses::G_BUFFER_PASS), VK_NULL_HANDLE);


		VkRect2D scissor = { 0,0,renderer_->GetSwapChainExtent() };

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, staticMeshPipeline_->GetPipeline());

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			staticMeshPipeline_->GetPipelineLayout(), 0, 1, &uboDescriptors_[threadID], 0, nullptr);

		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator it = meshInstances_.begin();
		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator end = meshInstances_.end();

		for (; it != end; ++it) {
			if (it->second->meshes == 0)
				continue;
			
			VkDescriptorSet materialDescriptor;

			materialDescriptor = it->second->material->GetMaterialDescriptorSet(threadID, staticMeshPipeline_->GetPipelineId(), 1);
			if (materialDescriptor == VK_NULL_HANDLE)
				materialDescriptor = it->second->material->CreateMaterialDescriptorSet(
					threadID, staticMeshPipeline_->GetPipelineId(), 
					1, staticMeshPipeline_->GetDescriptorSetLayout(1));

			eastl::shared_ptr<VulkanMesh> mesh = it->first;

			it->second->transformBuffer->UpdateBuffer(it->second->transforms.data(), 0,
				static_cast<uint32_t>(sizeof(glm::mat4)*it->second->meshes));

			VkBuffer buffers[] = { mesh->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

			buffers[0] = it->second->transformBuffer->GetBuffer();
			vkCmdBindVertexBuffers(commandBuffer, 1, 1, buffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, mesh->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, staticMeshPipeline_->GetPipelineLayout(),
				1, 1, &(materialDescriptor), 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, mesh->GetIndexCount(), it->second->meshes, 0, 0, 0);
		}

		renderer_->EndSecondaryCommandBufferRecording(commandBuffer);

		//*(buffer) = commandBuffer;
	}

	void VulkanStaticMeshRenderer::RenderShadows(size_t threadID, VkCommandPool commandPool, VkCommandBuffer buffer, uint32_t lightOffset)
	{
/*
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;*/

		VkCommandBuffer commandBuffer = buffer;

		//vkAllocateCommandBuffers(device->GetDevice(), &allocInfo, &commandBuffer);

		renderer_->StartSecondaryCommandBufferRecording(commandBuffer,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
			renderer_->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS), renderer_->GetFrameBuffer());

		VkRect2D scissor = { 0,0,renderer_->GetSwapChainExtent() };

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline_->GetPipeline());

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			shadowPipeline_->GetPipelineLayout(), 0, 1, &uboDescriptors_[threadID], 0, nullptr);

		VkDescriptorSet lightDescriptor = renderer_->GetLightDescriptorSet(threadID, shadowPipeline_->GetPipelineId(), 1);
		if (lightDescriptor == VK_NULL_HANDLE)
			lightDescriptor = renderer_->CreateLightDescriptorSet(threadID, shadowPipeline_->GetPipelineId(), 1, shadowPipeline_->GetDescriptorSetLayout(1));

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			shadowPipeline_->GetPipelineLayout(), 1, 1, &lightDescriptor, 1, &lightOffset);

		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator it = meshInstances_.begin();
		eastl::multimap<eastl::shared_ptr<VulkanMesh>, MeshData*>::iterator end = meshInstances_.end();

		for (; it != end; ++it) {
			if (it->second->meshes == 0)
				continue;

			eastl::shared_ptr<VulkanMesh> mesh = it->first;
			
			VkBuffer buffers[] = { mesh->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

			buffers[0] = it->second->transformBuffer->GetBuffer();
			vkCmdBindVertexBuffers(commandBuffer, 1, 1, buffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, mesh->GetShadowIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(commandBuffer, mesh->GetShadowIndexCount(), it->second->meshes, 0, 0, 0);
		}

		renderer_->EndSecondaryCommandBufferRecording(commandBuffer);

		//*(buffer) = commandBuffer;
	}

	void VulkanStaticMeshRenderer::Clean() const
	{
		staticMeshPipeline_->Clean();
		shadowPipeline_->Clean();
	}

	void VulkanStaticMeshRenderer::Recreate()
	{
		staticMeshPipeline_->SetRenderPassInfo(renderer_->GetGBufferRenderPass(), static_cast<int>(VulkanRenderer::GBufferSubPasses::G_BUFFER_PASS));
		staticMeshPipeline_->Recreate();

		shadowPipeline_->SetRenderPassInfo(renderer_->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS));
		shadowPipeline_->Recreate();
	}

}

#endif // USING_VULKAN