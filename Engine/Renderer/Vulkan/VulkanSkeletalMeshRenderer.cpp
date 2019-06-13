#include "VulkanSkeletalMeshRenderer.hpp"
#ifdef USING_VULKAN
#include "Engine/Renderer/VulkanRenderer.hpp"
#include "Engine/engine.hpp"
#include "Engine/Material/VulkanMaterial.hpp"

namespace Engine {

	VulkanSkeletalMeshRenderer::VulkanSkeletalMeshRenderer(VulkanRenderer* renderer, VulkanLogicalDevice* device, VulkanDescriptorPool* descriptorPool)
	{
		this->renderer_ = renderer;
		this->device_ = device;
		this->descriptorPool_ = descriptorPool;
		this->commandPool_ = renderer->GetGraphicsCommandPool();
		this->allocator_ = renderer->GetVmaAllocator();

		skeletalMeshPipeline_ = eastl::unique_ptr<VulkanPipeline>(new VulkanPipeline(device, renderer));

		skeletalMeshPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "SkeletonMesh.vert.spv");
		//skeletalMeshPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::GEOMETRY_SHADER, "Mesh.geom.spv");
		skeletalMeshPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "Mesh.frag.spv");

		skeletalMeshPipeline_->AddVertexInputBindingDescription(0, static_cast<uint32_t>(sizeof(Vertex)), VK_VERTEX_INPUT_RATE_VERTEX);
		skeletalMeshPipeline_->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
		skeletalMeshPipeline_->AddVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
		skeletalMeshPipeline_->AddVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords));
		skeletalMeshPipeline_->AddVertexInputAttributeDescription(3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, boneWeights));
		skeletalMeshPipeline_->AddVertexInputAttributeDescription(4, 0, VK_FORMAT_R32G32B32A32_SINT, offsetof(Vertex, boneIds));


		skeletalMeshPipeline_->CreateDescriptorSet();
		skeletalMeshPipeline_->AddDescriptorSetLayout(VulkanMaterial::CreateMaterialDescriptorSetLayout(device));
		skeletalMeshPipeline_->AddDescriptorSetLayout(renderer_->CreateLightDescriptorSetLayout());
		skeletalMeshPipeline_->CreateDescriptorSet();
		skeletalMeshPipeline_->CreateDescriptorSet();

		skeletalMeshPipeline_->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT, nullptr);
		skeletalMeshPipeline_->AddDescriptorSetBinding(3, 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr);
		skeletalMeshPipeline_->AddDescriptorSetBinding(4, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr);

		skeletalMeshPipeline_->AddPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, static_cast<uint32_t>(sizeof(PushConstants_t)));
		
		skeletalMeshPipeline_->SetRenderPassInfo(renderer->GetGBufferRenderPass(), static_cast<int>(VulkanRenderer::GBufferSubPasses::G_BUFFER_PASS));

		skeletalMeshPipeline_->CreateColorBlendAttachment(VK_FALSE);
		skeletalMeshPipeline_->CreateColorBlendAttachment(VK_FALSE);
		skeletalMeshPipeline_->CreateColorBlendAttachment(VK_FALSE);

		skeletalMeshPipeline_->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);

		skeletalMeshPipeline_->SetRasterizerSettings();

		skeletalMeshPipeline_->Compile();

		shadowPipeline_ = eastl::unique_ptr<VulkanPipeline>(new VulkanPipeline(device_, renderer_));

		shadowPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "SkeletonShadow.vert.spv");
		shadowPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::GEOMETRY_SHADER, "ShadowVolume.geom.spv");
		shadowPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "ShadowVolume.frag.spv");

		shadowPipeline_->AddVertexInputBindingDescription(0,
			static_cast<uint32_t>(sizeof(Vertex)), VK_VERTEX_INPUT_RATE_VERTEX);

		shadowPipeline_->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
		shadowPipeline_->AddVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
		shadowPipeline_->AddVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords));
		shadowPipeline_->AddVertexInputAttributeDescription(3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, boneWeights));
		shadowPipeline_->AddVertexInputAttributeDescription(4, 0, VK_FORMAT_R32G32B32A32_SINT, offsetof(Vertex, boneIds));
		
		shadowPipeline_->CreateDescriptorSet();
		shadowPipeline_->AddDescriptorSetLayout(renderer_->CreateLightDescriptorSetLayout());
		shadowPipeline_->CreateDescriptorSet();
		shadowPipeline_->CreateDescriptorSet();
		shadowPipeline_->CreateDescriptorSet();

		shadowPipeline_->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT, nullptr);
		shadowPipeline_->AddDescriptorSetBinding(3, 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr);
		shadowPipeline_->AddDescriptorSetBinding(4, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr);

		shadowPipeline_->AddPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT, 
			0, static_cast<uint32_t>(sizeof(PushConstants_t)));

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
		
		shadowPipeline_->CreateColorBlendAttachment(VK_TRUE, 0U);

		shadowPipeline_->Compile();

		ubo_ = { glm::mat4(), glm::mat4() };
		
		uniformBuffer_ = eastl::unique_ptr<VulkanBuffer>(new VulkanBuffer(device, renderer->GetVmaAllocator(),
			static_cast<uint32_t>(sizeof(ubo_)), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, false, renderer->GetGraphicsCommandPool()));

		uniformBuffer_->UpdateBuffer(&ubo_, 0, static_cast<uint32_t>(sizeof(ubo_)));

		uboDescriptors_.resize(renderer_->GetThreadCount());

		for (size_t i = 0, size = uboDescriptors_.size(); i < size; ++i) {

			VkDescriptorSetLayout layouts[] = { skeletalMeshPipeline_->GetDescriptorSetLayout(0) };
			renderer_->GetDescriptorPool(i)->AllocateDescriptorSet(1, layouts, &uboDescriptors_[i]);

			renderer_->GetDescriptorPool(i)->DescriptorSetBindToBuffer(uboDescriptors_[i], uniformBuffer_->GetBuffer(),
				0, static_cast<VkDeviceSize>(sizeof(ubo_)), 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
		}

		Engine::GetEngine().lock()->GetResourceManager().lock()->CreateTexture("default.png");
		defaultTexture_ = eastl::dynamic_pointer_cast<VulkanTexture, Texture>(
			Engine::GetEngine().lock()->GetResourceManager().lock()->GetTexture("default.png").lock());
	}


	VulkanSkeletalMeshRenderer::~VulkanSkeletalMeshRenderer()
	{
		uniformBuffer_.reset();

		skeletalMeshPipeline_.reset();
	}

	void VulkanSkeletalMeshRenderer::StartRender(glm::mat4 view, glm::mat4 projection) {		
		if (ubo_.view != view || ubo_.proj != projection) {
			ubo_.view = view;
			ubo_.proj = projection;

			uniformBuffer_->UpdateBuffer(&ubo_, 0, static_cast<uint32_t>(sizeof(ubo_)));
		}

		meshes.clear();
	}

	void VulkanSkeletalMeshRenderer::RenderMesh(const glm::mat4x4& modelMatrix, VulkanMesh* mesh,
		VulkanMaterial* material, Skeleton* skeleton, size_t animation,
		float time, float ticksPerSecond, float duration, bool looping, const glm::vec4 & mainColor)
	{		
			MeshData data = {};
			data.animation = animation;
			data.skeleton = skeleton;
			data.transform = modelMatrix;
			data.material = material;
			data.color = mainColor;
			data.mesh = mesh;
			data.time = time;
			data.ticksPerSecond = ticksPerSecond;
			data.duration = duration;
			data.looping = looping;
			data.boneData = static_cast<VulkanTexture*>(skeleton->GetAnimationData(animation));

			meshes.push_back(data);
	}

	void VulkanSkeletalMeshRenderer::FinishRender(size_t threadID, VkCommandPool commandPool, VkCommandBuffer buffer)
	{
		/*VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
*/
		VkCommandBuffer commandBuffer = buffer;
/*
		vkAllocateCommandBuffers(device->GetDevice(), &allocInfo, &commandBuffer);*/

		renderer_->StartSecondaryCommandBufferRecording(commandBuffer,
			VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			renderer_->GetGBufferRenderPass(), static_cast<int>(VulkanRenderer::GBufferSubPasses::G_BUFFER_PASS), VK_NULL_HANDLE);


		VkRect2D scissor = { 0,0,renderer_->GetSwapChainExtent() };

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skeletalMeshPipeline_->GetPipeline());

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			skeletalMeshPipeline_->GetPipelineLayout(), 0, 1, &uboDescriptors_[threadID], 0, nullptr);

		for (size_t i = 0, size = meshes.size(); i<size; ++i) {
			
			VulkanMesh* mesh = meshes[i].mesh;

			PushConstants_t constants = {};
			constants.model = meshes[i].transform;
			constants.color = meshes[i].color;
			constants.duration = meshes[i].duration;
			constants.looping = meshes[i].looping;
			constants.ticksPerSecond = meshes[i].ticksPerSecond;
			constants.time = meshes[i].time;
			

			vkCmdPushConstants(commandBuffer, skeletalMeshPipeline_->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
				static_cast<uint32_t>(sizeof(PushConstants_t)), &constants);
			
			VkBuffer buffers[] = { mesh->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, mesh->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			VkDescriptorSet materialDescriptor;

			materialDescriptor = meshes[i].material->GetMaterialDescriptorSet(threadID, skeletalMeshPipeline_->GetPipelineId(), 1);
			if (materialDescriptor == VK_NULL_HANDLE)
				materialDescriptor = meshes[i].material->CreateMaterialDescriptorSet(
					threadID, skeletalMeshPipeline_->GetPipelineId(),
					1, skeletalMeshPipeline_->GetDescriptorSetLayout(1));

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skeletalMeshPipeline_->GetPipelineLayout(),
				1, 1, &(materialDescriptor), 0, nullptr);
			
			VkDescriptorSet boneOffsets = mesh->GetBoneOffsetDescriptorSet(threadID, skeletalMeshPipeline_->GetPipelineId(), 4);

			if (boneOffsets == VK_NULL_HANDLE)
				boneOffsets = mesh->CreateBoneOffsetDescriptorSet(
					threadID, skeletalMeshPipeline_->GetPipelineId(),
					4, skeletalMeshPipeline_->GetDescriptorSetLayout(4));

			VkDescriptorSet boneDescriptor = meshes[i].boneData->GetDescriptorSet(threadID, skeletalMeshPipeline_->GetPipelineId(), 3);
			if (boneDescriptor == VK_NULL_HANDLE)
				boneDescriptor = meshes[i].boneData->CreateDescriptorSet(threadID, skeletalMeshPipeline_->GetPipelineId(), 
					3, skeletalMeshPipeline_->GetDescriptorSetLayout(3));

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skeletalMeshPipeline_->GetPipelineLayout(), 
				3, 1, &(boneDescriptor), 0, nullptr);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skeletalMeshPipeline_->GetPipelineLayout(), 4, 1, &boneOffsets, 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, mesh->GetIndexCount(), 1, 0, 0, 0);
		}

		renderer_->EndSecondaryCommandBufferRecording(commandBuffer);

	}

	void VulkanSkeletalMeshRenderer::RenderShadows(size_t threadID, VkCommandPool commandPool, VkCommandBuffer buffer, uint32_t lightOffset)
	{/*
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;*/

		VkCommandBuffer commandBuffer = buffer;
/*
		vkAllocateCommandBuffers(device->GetDevice(), &allocInfo, &commandBuffer);*/

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

		for (size_t i = 0, size = meshes.size(); i < size; ++i) {

			VulkanMesh* mesh = meshes[i].mesh;

			PushConstants_t constants = {};
			constants.model = meshes[i].transform;
			constants.color = meshes[i].color;
			constants.duration = meshes[i].duration;
			constants.looping = meshes[i].looping;
			constants.ticksPerSecond = meshes[i].ticksPerSecond;
			constants.time = meshes[i].time;


			vkCmdPushConstants(commandBuffer, shadowPipeline_->GetPipelineLayout(), 
				VK_SHADER_STAGE_VERTEX_BIT |VK_SHADER_STAGE_GEOMETRY_BIT, 0,
				static_cast<uint32_t>(sizeof(PushConstants_t)), &constants);

			VkBuffer buffers[] = { mesh->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, mesh->GetShadowIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			VkDescriptorSet boneOffsets = mesh->GetBoneOffsetDescriptorSet(threadID, shadowPipeline_->GetPipelineId(), 4);

			if (boneOffsets == VK_NULL_HANDLE)
				boneOffsets = mesh->CreateBoneOffsetDescriptorSet(
					threadID, shadowPipeline_->GetPipelineId(), 
					4, shadowPipeline_->GetDescriptorSetLayout(4));

			VkDescriptorSet boneDescriptor = meshes[i].boneData->GetDescriptorSet(threadID, shadowPipeline_->GetPipelineId(), 3);
			if (boneDescriptor == VK_NULL_HANDLE)
				boneDescriptor = meshes[i].boneData->CreateDescriptorSet(threadID, shadowPipeline_->GetPipelineId(),
					3, shadowPipeline_->GetDescriptorSetLayout(3));

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline_->GetPipelineLayout(),
				3, 1, &(boneDescriptor), 0, nullptr);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline_->GetPipelineLayout(), 4, 1, &boneOffsets, 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, mesh->GetShadowIndexCount(), 1, 0, 0, 0);
		}

		renderer_->EndSecondaryCommandBufferRecording(commandBuffer);

	}

	void VulkanSkeletalMeshRenderer::Clean() const
	{
		skeletalMeshPipeline_->Clean();
		shadowPipeline_->Clean();
	}

	void VulkanSkeletalMeshRenderer::Recreate() {
		skeletalMeshPipeline_->SetRenderPassInfo(renderer_->GetGBufferRenderPass(), static_cast<int>(VulkanRenderer::GBufferSubPasses::G_BUFFER_PASS));
		skeletalMeshPipeline_->Recreate();

		shadowPipeline_->SetRenderPassInfo(renderer_->GetRenderPass(), static_cast<int>(VulkanRenderer::RenderSubPasses::RENDER_PASS));
		shadowPipeline_->Recreate();
	}
}

#endif // USING_VULKAN