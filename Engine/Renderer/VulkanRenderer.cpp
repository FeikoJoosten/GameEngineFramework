// Written by Koen Buitenhuis

#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#define VMA_IMPLEMENTATION
#include "Engine/Renderer/VulkanRenderer.hpp"
#include "Engine/Renderer/imgui_impl_glfw_vulkan.h"
#include "Engine/engine.hpp"
//#include <chrono>
//#include <stdexcept>
#include <cstring>
#include <thread>
//#include <fstream>
//#include <set>
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>
#include "Engine/Mesh/VulkanMesh.hpp"
#include "Engine/Texture/VulkanTexture.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>


namespace Engine
{

	static void check_vk_result(VkResult err) {
		if (err != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(err).c_str() + "] ImGui vulkan call failed";
			std::cout << s.c_str() << std::endl;
		}
	}

	VulkanRenderer::VulkanRenderer() noexcept
	{
		initialized = false;
		setClearColor(glm::vec4(0.f, 0.f, 0.f, 0.f));
	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkWaitForFences(vulkanLogicalDevice_->GetDevice(),
			static_cast<uint32_t>(drawingFinishedFences_.size()),
			drawingFinishedFences_.data(),
			VK_TRUE, eastl::numeric_limits<uint64_t>::max());

		vkDeviceWaitIdle(vulkanLogicalDevice_->GetDevice());

		vkResetCommandPool(vulkanLogicalDevice_->GetDevice(), graphicsCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
		vkResetCommandPool(vulkanLogicalDevice_->GetDevice(), computeCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

		vkDeviceWaitIdle(vulkanLogicalDevice_->GetDevice());

		DestroyThreads();
		DestroyRenderers();
		DestroyGBufferRenderPipeline();
		DestroyCompositingPipeline();
		DestroySemaphores();
		DestroyImGui();
		DestroyDecriptorPool();
		DestroyFramebuffers();
		DestroyRenderPass();
		DestroyAttachmentImages();
		DestroyDepthImage();
		DestroyVmaAllocator();
		DestroyGraphicsCommandPool();
		DestroyComputeCommandPool();
		DestroySwapChainImageViews();
		DestroySwapChain();
		DestroyLogicalDevice();
		DestroyInstance();
	}

	void VulkanRenderer::InitializeRenderer()
	{
		InitializeRenderer(Engine::GetEngine().lock()->GetWindow<VulkanWindow>());
	}

	void VulkanRenderer::InitializeRenderer(eastl::weak_ptr<VulkanWindow> window)
	{
		if (initialized)
			return;

		this->window = window;

		swapChain = VK_NULL_HANDLE;

		CreateInstance();

		surface = window.lock()->CreateSurface(vulkanInstance_->GetInstance());

		FindPhysicalDevice();

		CreateLogicalDevice();

		CreateSwapChain();

		GetSwapChainImages();
		CreateSwapChainImageViews();

		CreateGraphicsCommandPool();
		CreateComputeCommandPool();

		CreateVmaAllocator();

		CreateDepthImage();

		CreateAttachmentImages();

		CreateRenderPass();

		CreateFramebuffers();

		CreateDescriptorPool();

		InitImGui();

		CreateSemaphores();

		CreateCompositingPipeline();

		CreateGBufferRenderPipeline();

		GenerateCommandBuffers();

		AllocateThreads();

		activeLights = 0;

		currentImage = 0;

		VulkanTexture::InitTextureSystem(this, vulkanLogicalDevice_.get(), vulkanDescriptorPools_[0].get(), vmaAllocator_, graphicsCommandPool);

		VulkanMesh::InitMeshes(this, vulkanLogicalDevice_.get(), vulkanDescriptorPools_[0].get(), graphicsCommandPool);

		VulkanMaterial::InitMaterialData(this, vulkanLogicalDevice_.get(), vulkanDescriptorPools_[0].get(), vmaAllocator_);

		CreateRenderers();

		/*Engine::GetEngine().lock()->GetResourceManager().lock()->CreateSkeleton("xbot.fbx");
		Engine::GetEngine().lock()->GetResourceManager().lock()->CreateModel("test", "xbot.fbx");
		Engine::GetEngine().lock()->GetResourceManager().lock()->AddAnimationsToSkeleton("xbot.fbx", "Standard Walk.fbx");
		Engine::GetEngine().lock()->GetResourceManager().lock()->GetModel("test")->SetAnimation(
		Engine::GetEngine().lock()->GetResourceManager().lock()->
		GetModel("test")->GetAnimations()[0], true);
		Engine::GetEngine().lock()->GetResourceManager().lock()->GetModel("test")->SetPaused(false);
		Engine::GetEngine().lock()->GetResourceManager().lock()->GetModel("test")->SetLooping(true);*/
		/*
		CreateLight("spot1", LightType::LIGHT_SPOT_LIGHT,
		glm::vec3(0.f,40.f,25.f), glm::normalize(glm::vec3(-0.25f, -0.75f, 0.5f)),
		glm::vec3(0.25f, 1.f, 0.25f), 800.f, 0.f, 1.f/10.f*glm::pi<float>(), 1.f/3.f*glm::pi<float>());*/
		/*
		CreateLight("dir1", LightType::LIGHT_DIRECTIONAL_LIGHT,
		glm::vec3(0.f), glm::normalize(glm::vec3(0.75f, -0.9f, 1.f)),
		glm::vec3(0.75f, 0.75f, 0.75f), 0.f, 0.f, 0.f, 0.f);*/

		//CreateLight("dir1", LightType::LIGHT_DIRECTIONAL_LIGHT,
		//	glm::vec3(25.f, 25.f, 10.f), glm::vec3(0.25f,0.25f,0.5f),
		//	glm::vec3(0.8f, 0.8f, 0.8f), 800.f, 0.f, glm::radians<float>(30.f), glm::radians<float>(70.f));


		//CreateLight("amb1", LightType::LIGHT_AMBIENT_LIGHT,
		//	glm::vec3(25.f, 25.f, 10.f), glm::vec3(0.25f, 0.25f, 0.5f),
		//	glm::vec3(0.2f, 0.2f, 0.2f), 800.f, 0.f, glm::radians<float>(30.f), glm::radians<float>(70.f));
		/*
		CreateLight("point1", LightType::LIGHT_SPOT_LIGHT,
		glm::vec3(25.f, 25.f, 10.f), glm::vec3(-0.25f, -0.25f, 0.5f),
		glm::vec3(0.8f, 0.8f, 0.8f), 800.f, 0.f, glm::radians<float>(30.f), glm::radians<float>(70.f));*/

		firstRender = true;

		initialized = true;

		resized = false;
	}

	void VulkanRenderer::Resized()
	{
		resized = true;
	}

	void VulkanRenderer::RecreateSwapChain()
	{
		vkDeviceWaitIdle(vulkanLogicalDevice_->GetDevice());

		DestroyFramebuffers();

		DestroyCompositingPipeline();
		DestroyGBufferRenderPipeline();
		vulkanSpriteRenderer->Clean();
		vulkanDebugRenderer->Clean();
		vulkanStaticMeshRenderer->Clean();
		vulkanSkeletalMeshRenderer->Clean();

		DestroyImGui();

		DestroyRenderPass();
		DestroyAttachmentImages();
		DestroyDepthImage();
		DestroySwapChainImageViews();
		DestroySwapChain();

		CreateSwapChain();
		GetSwapChainImages();
		CreateSwapChainImageViews();
		CreateDepthImage();
		CreateAttachmentImages();
		CreateRenderPass();
		CreateFramebuffers();

		CreateCompositingPipeline();
		CreateGBufferRenderPipeline();

		vulkanSpriteRenderer->Recreate();
		vulkanDebugRenderer->Recreate();
		vulkanStaticMeshRenderer->Recreate();
		vulkanSkeletalMeshRenderer->Recreate();

		InitImGui();

		GenerateCommandBuffers();

		resized = false;

	}

	void VulkanRenderer::setClearColor(glm::vec4 color)
	{
		clearValue.color = { color.x,color.y,color.z,color.w };
	}

	VkRenderPass VulkanRenderer::GetRenderPass() const
	{
		return renderPass;
	}

	VkRenderPass VulkanRenderer::GetGBufferRenderPass() const
	{
		return gBufferPass;
	}

	VmaAllocator VulkanRenderer::GetVmaAllocator() const
	{
		return vmaAllocator_;
	}

	VkCommandPool VulkanRenderer::GetGraphicsCommandPool() const
	{
		return graphicsCommandPool;
	}

	VkCommandPool VulkanRenderer::GetComputeCommandPool() const
	{
		return computeCommandPool;
	}

	VkExtent2D VulkanRenderer::GetSwapChainExtent() const
	{
		return swapChainImageExtent;
	}

	void VulkanRenderer::RendererBegin()
	{
		RendererBegin(glm::mat4(), glm::perspective(75.f,
			static_cast<float>(swapChainImageExtent.width) / static_cast<float>(swapChainImageExtent.height),
			0.1f, 1000.f));
	}

	void VulkanRenderer::RendererBegin(const glm::mat4x4 & view, const glm::mat4x4 & projection)
	{
		//vkDeviceWaitIdle(vulkanLogicalDevice->GetDevice());
		uint32_t imageIndex;
		VkResult res = vkAcquireNextImageKHR(vulkanLogicalDevice_->GetDevice(), swapChain, eastl::numeric_limits<uint64_t>::max(), imageAvailableSemaphores_[(currentImage + 1) % imageAvailableSemaphores_.size()], VK_NULL_HANDLE, &imageIndex);

		if (res == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
		}
		else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to aquire next swap chain image";
			std::cout << s.c_str() << std::endl;
			return;
		}

		currentBuffer_ = primaryRenderCommandBuffers_[imageIndex];
		currentComputeBuffer_ = primaryComputeCommandBuffers_[imageIndex];
		this->prevImage = this->currentImage;
		this->currentImage = imageIndex;

		//vkQueueWaitIdle(vulkanLogicalDevice_->GetComputeQueue());
		VkFence fences[] = { drawingFinishedFences_[currentImage], computeFinishedFences_[currentImage] };
		vkWaitForFences(vulkanLogicalDevice_->GetDevice(), 2, fences, true, eastl::numeric_limits<uint64_t>::max());
		vkResetFences(vulkanLogicalDevice_->GetDevice(), 2, fences);

		/*VkResult res = vkQueueWaitIdle(vulkanLogicalDevice->GetGraphicsQueue());
		if (res != VK_SUCCESS) {
		eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Queue wait idle failed";
		std::cout << s.c_str() << std::endl;
		}*/

		if (resized) {
			VkResult res = vkQueueWaitIdle(vulkanLogicalDevice_->GetGraphicsQueue());
			if (res != VK_SUCCESS) {
				eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Queue wait idle failed";
				std::cout << s.c_str() << std::endl;
			}
			RecreateSwapChain();
		}

		ImGui_ImplGlfwVulkan_NewFrame();


		vulkanSpriteRenderer->StartRender(spriteRenderCommandBuffers_[imageIndex], view, projection);
		vulkanDebugRenderer->StartRender(debugRenderCommandBuffers_[imageIndex], view, projection);
		vulkanStaticMeshRenderer->StartRender(view, projection);
		vulkanSkeletalMeshRenderer->StartRender(view, projection);

		glm::vec3 camPos = Engine::GetEngine().lock()->GetCamera().lock()->GetPosition();
		scene.viewPos = glm::vec4(camPos.x, camPos.y, camPos.z, 1.f);
		/*
		Engine::GetEngine().lock()->GetResourceManager().lock()->GetModel("test")->UpdateAnimation(
		Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime());

		Render(glm::translate(glm::mat4(), glm::vec3(-50.f, -200.f, -150.f)), Engine::GetEngine().lock()->GetResourceManager().lock()->GetModel("test"));
		Render(glm::translate(glm::mat4(), glm::vec3(50.f, -200.f, -150.f)), Engine::GetEngine().lock()->GetResourceManager().lock()->GetModel("test"));*/

		//RenderSprite(texture, glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -1.f))*glm::scale(glm::mat4(),glm::vec3(64.f,64.f,1.f)));
		//RenderSprite(texture, glm::translate(glm::mat4(), glm::vec3(40.f, 0.f, -40.f))*glm::scale(glm::mat4(), glm::vec3(64.f, 64.f, 1.f)));

		//RenderSprite(texture, glm::mat4());
	}

	void VulkanRenderer::Render()
	{

	}

	void VulkanRenderer::Render(const glm::mat4x4 & modelMatrix, eastl::shared_ptr<Model> model, const glm::vec4 & mainColor)
	{
		eastl::vector<eastl::shared_ptr<Mesh>> meshes = model->GetModelMeshes();

		for (size_t i = 0, size = meshes.size(); i < size; i++) {
			eastl::shared_ptr<VulkanMaterial> material =
				eastl::dynamic_pointer_cast<VulkanMaterial, Material>(model->GetMeshMaterial(meshes[i]));

			if (!eastl::dynamic_pointer_cast<VulkanMesh, Mesh>(meshes[i])->IsAnimated() ||
				model->GetCurrentAnimationIndex() == -1 ||
				model->GetSkeleton() == nullptr) {
				vulkanStaticMeshRenderer->RenderMesh(modelMatrix,
					eastl::dynamic_pointer_cast<VulkanMesh, Mesh>(meshes[i]), material, mainColor);
			}
			else {
				vulkanSkeletalMeshRenderer->RenderMesh(modelMatrix,
					static_cast<VulkanMesh*>(meshes[i].get()),
					material.get(), model->GetSkeleton().get(), model->GetCurrentAnimationIndex(), model->GetAnimationTime(),
					model->GetSkeleton()->GetAnimationTicksPerSecond(model->GetCurrentAnimationIndex()),
					model->GetAnimationDuration(), model->IsLooping(), mainColor);
			}
		}
	}

	void VulkanRenderer::RenderSprite(eastl::weak_ptr<Texture> texture, glm::mat4 modelMatrix)
	{
		eastl::weak_ptr<VulkanTexture> vulkanTexture;

		vulkanTexture = eastl::dynamic_pointer_cast<VulkanTexture, Texture>(texture.lock());

		vulkanSpriteRenderer->RenderSprite(vulkanTexture, modelMatrix);
	}

	void VulkanRenderer::RenderLine(glm::vec3 start, glm::vec3 end, glm::vec4 color)
	{
		vulkanDebugRenderer->RenderLine(start, end, color);
	}

	void VulkanRenderer::RendererEnd()
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pInheritanceInfo = nullptr;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vkBeginCommandBuffer(currentComputeBuffer_, &beginInfo);

		// TODO - add compute stuff

		vkEndCommandBuffer(currentComputeBuffer_);

		VkSubmitInfo computeSubmitInfo = {};
		computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		computeSubmitInfo.waitSemaphoreCount = 0;
		computeSubmitInfo.pWaitSemaphores = nullptr;
		computeSubmitInfo.pWaitDstStageMask = nullptr;
		computeSubmitInfo.commandBufferCount = 1;
		computeSubmitInfo.pCommandBuffers = &currentComputeBuffer_;
		computeSubmitInfo.signalSemaphoreCount = 1;
		VkSemaphore computeSignalSemaphores[] = { computeFinishedSemaphores_[currentImage] };
		computeSubmitInfo.pSignalSemaphores = computeSignalSemaphores;

		VkResult res = vkQueueSubmit(vulkanLogicalDevice_->GetComputeQueue(), 1, &computeSubmitInfo, computeFinishedFences_[currentImage]);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to submit compute commands";
			std::cout << s.c_str() << std::endl;
		}

		beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pInheritanceInfo = nullptr;
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vkBeginCommandBuffer(currentBuffer_, &beginInfo);

		eastl::array<VkClearValue, static_cast<int>(GBufferAttachments::ATTACHMENT_COUNT)> gbufferClearValues = {};
		gbufferClearValues[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].color = clearValue.color;
		gbufferClearValues[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].color = { 0.f,0.f,0.f,0.f };
		gbufferClearValues[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].color = { 0.f,0.f,0.f,0.f };
		gbufferClearValues[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].depthStencil = { 1.0f,0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = gBufferPass;
		renderPassInfo.framebuffer = gBufferFrameBuffer;
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChainImageExtent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(gbufferClearValues.size());
		renderPassInfo.pClearValues = gbufferClearValues.data();

		vkCmdBeginRenderPass(currentBuffer_, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		/*ThreadInfo* freeThread = GetFreeThread();
		if (freeThread != nullptr) {
		freeThread->task = std::packaged_task<void()>(std::bind(&VulkanStaticMeshRenderer::FinishRender, vulkanStaticMeshRenderer.get(),
		freeThread->threadId, freeThread->commandPool, &(staticMeshCommandBuffers_[currentImage])));
		freeThread->future = freeThread->task.get_future();
		freeThread->thread = std::thread(std::move(freeThread->task));
		freeThread->initialized = true;
		}
		else {*/
		vulkanStaticMeshRenderer->FinishRender(0, graphicsCommandPool, (staticMeshCommandBuffers_[currentImage]));
		//}
		/*freeThread = GetFreeThread();
		if (freeThread != nullptr) {
		freeThread->task = std::packaged_task<void()>(std::bind(&VulkanSkeletalMeshRenderer::FinishRender, vulkanSkeletalMeshRenderer.get(),
		freeThread->threadId, freeThread->commandPool, &(skeletalMeshCommandBuffers_[currentImage])));
		freeThread->future = freeThread->task.get_future();
		freeThread->thread = std::thread(std::move(freeThread->task));
		freeThread->initialized = true;
		}
		else {*/
		vulkanSkeletalMeshRenderer->FinishRender(0, graphicsCommandPool, (skeletalMeshCommandBuffers_[currentImage]));
		//}

		vkCmdExecuteCommands(currentBuffer_, 1, &staticMeshCommandBuffers_[currentImage]);

		vkCmdExecuteCommands(currentBuffer_, 1, &skeletalMeshCommandBuffers_[currentImage]);

		vkCmdEndRenderPass(currentBuffer_);

		eastl::array<VkClearValue, static_cast<int>(RenderPassAttachments::ATTACHMENT_COUNT)> renderClearValues = {};
		renderClearValues[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].color = clearValue.color;
		renderClearValues[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].color = { 0.f,0.f,0.f,0.f };
		//clearValues[SubPasses::RENDER_PASS].depthStencil = { 1.f,0 };
		renderClearValues[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].color = clearValue.color;
		//clearValues[SubPasses::IMGUI_PASS].depthStencil = { 1.f,0 };
		renderClearValues[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].color = clearValue.color;

		renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffers[currentImage];
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChainImageExtent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(renderClearValues.size());
		renderPassInfo.pClearValues = renderClearValues.data();

		vkCmdBeginRenderPass(currentBuffer_, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		StartSecondaryCommandBufferRecording(imguiCommandBuffers_[currentImage],
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
			renderPass, static_cast<int>(RenderSubPasses::IMGUI_PASS), framebuffers[currentImage]);

		ImGui_ImplGlfwVulkan_Render(imguiCommandBuffers_[currentImage]);
		EndSecondaryCommandBufferRecording(imguiCommandBuffers_[currentImage]);

		vulkanSpriteRenderer->FinishRender();
		vulkanDebugRenderer->FinishRender();

		if (lightDataRestructured)
			RestructureLights();
		if (lightDataChanged)
			UpdateLightData();

		scene.lightCount = activeLights;
		sceneDataBuffer->UpdateBuffer(&scene, 0, static_cast<uint32_t>(sizeof(scene)));

		uint32_t size = static_cast<uint32_t>(staticMeshShadowCommandBuffers_[currentImage].size());

		if (size < static_cast<uint32_t>(activeLights)) {
			staticMeshShadowCommandBuffers_[currentImage].resize(activeLights);
			GenerateSecondaryCommandBuffers(staticMeshShadowCommandBuffers_[currentImage].data() + sizeof(VkCommandBuffer)*size,
				static_cast<uint32_t>(staticMeshShadowCommandBuffers_[currentImage].size()) - size);
		}

		size = static_cast<uint32_t>(skeletalMeshShadowCommandBuffers_[currentImage].size());

		if (size < static_cast<uint32_t>(activeLights)) {
			skeletalMeshShadowCommandBuffers_[currentImage].resize(activeLights);
			GenerateSecondaryCommandBuffers(skeletalMeshShadowCommandBuffers_[currentImage].data() + sizeof(VkCommandBuffer)*size,
				static_cast<uint32_t>(skeletalMeshShadowCommandBuffers_[currentImage].size()) - size);
		}

		size = static_cast<uint32_t>(gBufferShadowCommandBuffers_[currentImage].size());

		if (size < static_cast<uint32_t>(activeLights)) {
			gBufferShadowCommandBuffers_[currentImage].resize(activeLights);
			GenerateSecondaryCommandBuffers(gBufferShadowCommandBuffers_[currentImage].data() + sizeof(VkCommandBuffer)*size,
				static_cast<uint32_t>(gBufferShadowCommandBuffers_[currentImage].size()) - size);
		}


		VkRect2D scissor = { 0,0,swapChainImageExtent };
		VkViewport viewport = { 0.f,0.f,static_cast<float>(swapChainImageExtent.width),static_cast<float>(swapChainImageExtent.height),-1.f,1.f };

		VkDescriptorSet sceneDescriptorSet = GetLightDescriptorSet(0, gBufferRenderPipeline_->GetPipelineId(), 1);
		if (sceneDescriptorSet == VK_NULL_HANDLE) {
			sceneDescriptorSet = CreateLightDescriptorSet(0, gBufferRenderPipeline_->GetPipelineId(), 1, gBufferRenderPipeline_->GetDescriptorSetLayout(1));
		}

		for (int i = 0; i < activeLights; ++i) {
			if (!(lightData[i].position.w == 0.f && lightData[i].direction == glm::vec4(0.f))) {
				/*freeThread = GetFreeThread();
				if (freeThread != nullptr) {
				freeThread->task = std::packaged_task<void()>(std::bind(&VulkanStaticMeshRenderer::RenderShadows, vulkanStaticMeshRenderer.get(),
				freeThread->threadId, freeThread->commandPool,
				&(staticMeshShadowCommandBuffers_[currentImage][i]), static_cast<uint32_t>(i * sizeof(Light))));
				freeThread->future = freeThread->task.get_future();
				freeThread->thread = std::thread(std::move(freeThread->task));
				freeThread->initialized = true;
				}
				else {*/
				vulkanStaticMeshRenderer->RenderShadows(0, graphicsCommandPool, (staticMeshShadowCommandBuffers_[currentImage][i]), static_cast<uint32_t>(i * sizeof(Light)));
				//}
				/*freeThread = GetFreeThread();
				if (freeThread != nullptr) {
				freeThread->task = std::packaged_task<void()>(std::bind(&VulkanSkeletalMeshRenderer::RenderShadows, vulkanSkeletalMeshRenderer.get(),
				freeThread->threadId, freeThread->commandPool,
				&(skeletalMeshShadowCommandBuffers_[currentImage][i]), static_cast<uint32_t>(i * sizeof(Light))));
				freeThread->future = freeThread->task.get_future();
				freeThread->thread = std::thread(std::move(freeThread->task));
				freeThread->initialized = true;
				}
				else {*/
				vulkanSkeletalMeshRenderer->RenderShadows(0, graphicsCommandPool, (skeletalMeshShadowCommandBuffers_[currentImage][i]), static_cast<uint32_t>(i * sizeof(Light)));
				//}
			}
			StartSecondaryCommandBufferRecording(gBufferShadowCommandBuffers_[currentImage][i],
				VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
				renderPass, static_cast<int>(RenderSubPasses::RENDER_PASS), framebuffers[currentImage]);

			vkCmdBindPipeline(gBufferShadowCommandBuffers_[currentImage][i], VK_PIPELINE_BIND_POINT_GRAPHICS, gBufferRenderPipeline_->GetPipeline());

			vkCmdSetScissor(gBufferShadowCommandBuffers_[currentImage][i], 0, 1, &scissor);

			VkDescriptorSet descriptors[] = { gBufferAttachmentDescriptorSet_ };

			vkCmdBindDescriptorSets(gBufferShadowCommandBuffers_[currentImage][i],
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				gBufferRenderPipeline_->GetPipelineLayout(),
				0, 1,
				descriptors,
				0, nullptr);

			uint32_t dynamicOffset[] = { static_cast<uint32_t>(i * sizeof(Light)) };

			vkCmdBindDescriptorSets(gBufferShadowCommandBuffers_[currentImage][i],
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				gBufferRenderPipeline_->GetPipelineLayout(),
				1, 1,
				&sceneDescriptorSet,
				1, dynamicOffset);


			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(gBufferShadowCommandBuffers_[currentImage][i], 0, 1, &vertexBuffer_, offsets);

			vkCmdDraw(gBufferShadowCommandBuffers_[currentImage][i], 6, 1, 0, 0);

			EndSecondaryCommandBufferRecording(gBufferShadowCommandBuffers_[currentImage][i]);
		}

		StartSecondaryCommandBufferRecording(clearStencilCommandBuffers_[currentImage],
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
			renderPass, static_cast<int>(RenderSubPasses::RENDER_PASS), framebuffers[currentImage]);

		VkClearValue clearValue = {};
		clearValue.depthStencil = { 0.f,0 };

		VkClearAttachment stencilAttachment = {};
		stencilAttachment.colorAttachment = static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT);
		stencilAttachment.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
		stencilAttachment.clearValue = clearValue;

		VkClearRect clearRect = {};
		clearRect.baseArrayLayer = 0;
		clearRect.layerCount = 1;
		clearRect.rect.offset = { 0,0 };
		clearRect.rect.extent = swapChainImageExtent;

		vkCmdClearAttachments(clearStencilCommandBuffers_[currentImage], 1, &stencilAttachment, 1, &clearRect);

		EndSecondaryCommandBufferRecording(clearStencilCommandBuffers_[currentImage]);

		JoinAllThreads();

		vkCmdNextSubpass(currentBuffer_, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		vkCmdExecuteCommands(currentBuffer_, 1, &spriteRenderCommandBuffers_[currentImage]);

		vkCmdExecuteCommands(currentBuffer_, 1, &debugRenderCommandBuffers_[currentImage]);

		for (int i = 0; i < activeLights; ++i) {
			vkCmdExecuteCommands(currentBuffer_, 1, &(clearStencilCommandBuffers_[currentImage]));

			if (!(lightData[i].position.w == 0.f && lightData[i].direction == glm::vec4(0.f))) {
				vkCmdExecuteCommands(currentBuffer_, 1, &(staticMeshShadowCommandBuffers_[currentImage][i]));
				vkCmdExecuteCommands(currentBuffer_, 1, &(skeletalMeshShadowCommandBuffers_[currentImage][i]));
			}
			vkCmdExecuteCommands(currentBuffer_, 1, &(gBufferShadowCommandBuffers_[currentImage][i]));
		}

		vkCmdNextSubpass(currentBuffer_, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		vkCmdExecuteCommands(currentBuffer_, 1, &imguiCommandBuffers_[currentImage]);

		vkCmdNextSubpass(currentBuffer_, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		vkCmdExecuteCommands(currentBuffer_, 1, &compositeCommandBuffer_);

		vkCmdEndRenderPass(currentBuffer_);

		res = vkEndCommandBuffer(currentBuffer_);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to record primary command buffer";
			std::cout << s.c_str() << std::endl;
			return;
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore waitSemaphores[] = {
			imageAvailableSemaphores_[(prevImage + 1) % imageAvailableSemaphores_.size()],
			computeFinishedSemaphores_[currentImage] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		submitInfo.waitSemaphoreCount = 2;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &currentBuffer_;
		VkSemaphore signalSemaphors[] = { renderFinishedSemaphores_[currentImage] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphors;

		res = vkQueueSubmit(vulkanLogicalDevice_->GetGraphicsQueue(), 1, &submitInfo, drawingFinishedFences_[currentImage]);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Submitting command buffer to queue failed";
			std::cout << s.c_str() << std::endl;
			return;
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphors;

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &currentImage;
		presentInfo.pResults = nullptr;

		res = vkQueuePresentKHR(vulkanLogicalDevice_->GetPresentQueue(), &presentInfo);
		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
			RecreateSwapChain();
		}
		else if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to present swap chain image";
			std::cout << s.c_str() << std::endl;
		}

		if (firstRender)
			firstRender = false;


	}

	void VulkanRenderer::CreateLight(eastl::string name, LightType lightType,
		glm::vec3 position, glm::vec3 direction, glm::vec3 color,
		float radius, float attunuation, float coneInnerAngle, float coneOuterAngle)
	{
		LightInfo info = {};
		info.active = true;
		info.name = name;
		info.type = lightType;

		switch (lightType) {
		case LightType::LIGHT_AMBIENT_LIGHT:
			info.light.position = glm::vec4(position.x, position.y, position.z, 0.f);
			info.light.direction = glm::vec4(0.f);
			info.light.color = glm::vec4(color.x, color.y, color.z, 1.f);
			info.light.radius = radius;
			info.light.attunuation = attunuation;
			info.light.coneInnerAngle = coneInnerAngle;
			info.light.coneOuterAngle = coneOuterAngle;
			break;
		case LightType::LIGHT_DIRECTIONAL_LIGHT:
			info.light.position = glm::vec4(position.x, position.y, position.z, 0.f);
			info.light.direction = glm::vec4(-direction.x, -direction.y, direction.z, 0.f);
			info.light.color = glm::vec4(color.x, color.y, color.z, 1.f);
			info.light.radius = radius;
			info.light.attunuation = attunuation;
			info.light.coneInnerAngle = coneInnerAngle;
			info.light.coneOuterAngle = coneOuterAngle;
			break;
		case LightType::LIGHT_POINT_LIGHT:
			info.light.position = glm::vec4(position.x, position.y, position.z, 1.f);
			info.light.direction = glm::vec4(-direction.x, -direction.y, direction.z, 0.f);
			info.light.color = glm::vec4(color.x, color.y, color.z, 1.f);
			info.light.radius = radius;
			info.light.attunuation = attunuation;
			info.light.coneInnerAngle = 2.f*glm::pi<float>();
			info.light.coneOuterAngle = 2.f*glm::pi<float>();
			break;
		case LightType::LIGHT_SPOT_LIGHT:
			info.light.position = glm::vec4(position.x, position.y, position.z, 1.f);
			info.light.direction = glm::vec4(-direction.x, -direction.y, direction.z, 0.f);
			info.light.color = glm::vec4(color.x, color.y, color.z, 1.f);
			info.light.radius = radius;
			info.light.attunuation = attunuation;
			info.light.coneInnerAngle = coneInnerAngle;
			info.light.coneOuterAngle = coneOuterAngle;
			break;
		}

		lights[name] = info;

		lightDataRestructured = true;

		scene.lightCount = activeLights;
	}

	void VulkanRenderer::SetLightType(eastl::string name, LightType lightType)
	{
		eastl::map<eastl::string, LightInfo>::iterator it = lights.find(name);

		if (it == lights.end())
			return;

		LightInfo info = it->second;

		if (info.type == lightType)
			return;

		if (info.type == LightType::LIGHT_NONEXISTENT) {
			info.type = lightType;
			info.active = true;
			lightDataRestructured = true;
		}

		if (lightType == LightType::LIGHT_NONEXISTENT) {
			info.type = lightType;
			info.active = false;
			lightDataRestructured = true;
			lights[name] = info;
		}

		info.type = lightType;

		switch (lightType) {
		case LightType::LIGHT_AMBIENT_LIGHT:
			info.light.position.w = 0.f;
			info.light.direction = glm::vec4(0.f);
			break;
		case LightType::LIGHT_DIRECTIONAL_LIGHT:
			info.light.position.w = 0.f;
			break;
		case LightType::LIGHT_POINT_LIGHT:
			info.light.position.w = 1.f;
			info.light.coneInnerAngle = 2.f * glm::pi<float>();
			info.light.coneOuterAngle = 2.f * glm::pi<float>();
			break;
		case LightType::LIGHT_SPOT_LIGHT:
			info.light.position.w = 1.f;
			info.light.coneInnerAngle = 0.f;
			info.light.coneOuterAngle = 0.f;
			break;
		}

		lights[name] = info;

		if (lightDataRestructured == false)
			lightData[info.lightDataIndex] = info.light;

		lightDataChanged = true;
	}

	LightType VulkanRenderer::GetLightType(eastl::string name)
	{
		if (lights.count(name) > 0)
			return lights[name].type;
		else
			return LightType::LIGHT_NONEXISTENT;
	}

	void VulkanRenderer::SetLightPosition(eastl::string name, glm::vec3 position)
	{
		if (lights.count(name) <= 0)
			return;

		LightInfo info = lights[name];

		info.light.position.x = position.x;
		info.light.position.y = position.y;
		info.light.position.z = position.z;

		if (info.active && (info.type == LightType::LIGHT_POINT_LIGHT || info.type == LightType::LIGHT_SPOT_LIGHT)) {
			lightData[info.lightDataIndex] = info.light;
			lightDataChanged = true;
		}

		lights[name] = info;
	}

	glm::vec3 VulkanRenderer::GetLightPosition(eastl::string name)
	{
		if (lights.count(name) <= 0)
			return glm::vec3(0.f);

		LightInfo info = lights[name];

		return glm::vec3(info.light.position.x, info.light.position.y, info.light.position.z);
	}

	void VulkanRenderer::SetLightDirection(eastl::string name, glm::vec3 direction)
	{
		if (lights.count(name) <= 0)
			return;

		LightInfo info = lights[name];


		if (info.active && (info.type == LightType::LIGHT_DIRECTIONAL_LIGHT || info.type == LightType::LIGHT_SPOT_LIGHT)) {
			info.light.direction.x = -direction.x;
			info.light.direction.y = -direction.y;
			info.light.direction.z = direction.z;

			lightData[info.lightDataIndex] = info.light;
			lightDataChanged = true;
		}

		lights[name] = info;
	}

	glm::vec3 VulkanRenderer::GetLightDirection(eastl::string name)
	{

		if (lights.count(name) <= 0)
			return glm::vec3(0.f);

		LightInfo info = lights[name];

		return glm::vec3(info.light.direction.x, info.light.direction.y, info.light.direction.z);
	}

	void VulkanRenderer::SetLightColor(eastl::string name, glm::vec3 color)
	{
		if (lights.count(name) <= 0)
			return;

		LightInfo info = lights[name];

		info.light.color.x = color.x;
		info.light.color.y = color.y;
		info.light.color.z = color.z;

		if (info.active) {
			lightData[info.lightDataIndex] = info.light;
			lightDataChanged = true;
		}

		lights[name] = info;
	}

	glm::vec3 VulkanRenderer::GetLightColor(eastl::string name)
	{
		if (lights.count(name) <= 0)
			return glm::vec3(0.f);

		LightInfo info = lights[name];

		return glm::vec3(info.light.color.x, info.light.color.y, info.light.color.z);
	}

	void VulkanRenderer::SetLightRadius(eastl::string name, float radius)
	{
		if (lights.count(name) <= 0)
			return;

		LightInfo info = lights[name];
		info.light.radius = radius;
		if (info.type == LightType::LIGHT_SPOT_LIGHT || info.type == LightType::LIGHT_POINT_LIGHT) {
			if (info.active) {
				lightData[info.lightDataIndex] = info.light;
				lightDataChanged = true;
			}
		}
		lights[name] = info;
	}

	float VulkanRenderer::GetLightRadius(eastl::string name)
	{
		if (lights.count(name) == 0)
			return 0.0f;

		return lights[name].light.radius;
	}

	void VulkanRenderer::SetLightConeInnerAngle(eastl::string name, float angle)
	{
		if (lights.count(name) <= 0)
			return;

		LightInfo info = lights[name];
		info.light.coneInnerAngle = angle;
		if (info.type == LightType::LIGHT_SPOT_LIGHT) {
			if (info.active) {
				lightData[info.lightDataIndex] = info.light;
				lightDataChanged = true;
			}
		}
		lights[name] = info;
	}

	float VulkanRenderer::GetLightConeInnerAngle(eastl::string name)
	{
		if (lights.count(name) == 0)
			return 0.0f;

		return lights[name].light.coneInnerAngle;
	}

	void VulkanRenderer::SetLightConeOuterAngle(eastl::string name, float angle)
	{
		if (lights.count(name) <= 0)
			return;

		LightInfo info = lights[name];
		info.light.coneOuterAngle = angle;
		if (info.type == LightType::LIGHT_SPOT_LIGHT) {
			if (info.active) {
				lightData[info.lightDataIndex] = info.light;
				lightDataChanged = true;
			}
		}
		lights[name] = info;
	}

	float VulkanRenderer::GetLightConeOuterAngle(eastl::string name)
	{
		if (lights.count(name) == 0)
			return 0.0f;

		return lights[name].light.coneOuterAngle;
	}

	VkDescriptorSetLayout VulkanRenderer::CreateLightDescriptorSetLayout()
	{

		eastl::array<VkDescriptorSetLayoutBinding, 2> descriptorSetBindings;

		descriptorSetBindings[0] = {};
		descriptorSetBindings[0].binding = 0;
		descriptorSetBindings[0].descriptorCount = 1;
		descriptorSetBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		descriptorSetBindings[0].pImmutableSamplers = nullptr;
		descriptorSetBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT;

		descriptorSetBindings[1] = {};
		descriptorSetBindings[1].binding = 1;
		descriptorSetBindings[1].descriptorCount = 1;
		descriptorSetBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetBindings[1].pImmutableSamplers = nullptr;
		descriptorSetBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT;

		VkDescriptorSetLayoutCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		info.bindingCount = static_cast<uint32_t>(descriptorSetBindings.size());
		info.pBindings = descriptorSetBindings.data();
		info.flags = 0;

		VkDescriptorSetLayout layout;
		vkCreateDescriptorSetLayout(vulkanLogicalDevice_->GetDevice(), &info, nullptr, &layout);

		return layout;
	}

	VkDescriptorSet VulkanRenderer::CreateLightDescriptorSet(size_t threadID, size_t pipelineID, size_t set, VkDescriptorSetLayout layout)
	{
		if (lightDescriptorSets_.size() <= pipelineID) {
			lightDescriptorSets_.resize(pipelineID + 1);
		}
		if (lightDescriptorSets_[pipelineID].size() <= set) {
			lightDescriptorSets_[pipelineID].resize(set + 1);
		}

		if (lightDescriptorSets_[pipelineID][set].size() <= threadID) {
			lightDescriptorSets_[pipelineID][set].resize(threadID + 1, VK_NULL_HANDLE);
		}

		if (threadID >= vulkanDescriptorPools_.size())
			return VK_NULL_HANDLE;

		VkDescriptorSetLayout layouts[] = { layout };
		vulkanDescriptorPools_[threadID]->AllocateDescriptorSet(1, layouts, &lightDescriptorSets_[pipelineID][set][threadID]);

		vulkanDescriptorPools_[threadID]->DescriptorSetBindToBuffer(lightDescriptorSets_[pipelineID][set][threadID], lightBuffer->GetBuffer(),
			0, static_cast<uint32_t>(sizeof(Light)), 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1);

		vulkanDescriptorPools_[threadID]->DescriptorSetBindToBuffer(lightDescriptorSets_[pipelineID][set][threadID], sceneDataBuffer->GetBuffer(),
			0, static_cast<uint32_t>(sizeof(SceneInfo)), 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);

		return lightDescriptorSets_[pipelineID][set][threadID];
	}

	VkDescriptorSet VulkanRenderer::GetLightDescriptorSet(size_t threadID, size_t pipelineID, size_t set)
	{
		if (pipelineID >= lightDescriptorSets_.size())
			return VK_NULL_HANDLE;
		if (set >= lightDescriptorSets_[pipelineID].size())
			return VK_NULL_HANDLE;
		if (threadID >= lightDescriptorSets_[pipelineID][set].size())
			return VK_NULL_HANDLE;
		return lightDescriptorSets_[pipelineID][set][threadID];
	}

	VkFramebuffer VulkanRenderer::GetFrameBuffer()
	{
		return framebuffers[currentImage];
	}

	VulkanDescriptorPool* VulkanRenderer::GetDescriptorPool(size_t thread)
	{
		if (thread >= vulkanDescriptorPools_.size())
			return nullptr;
		return vulkanDescriptorPools_[thread].get();
	}

	size_t VulkanRenderer::GetThreadCount()
	{
		return threads.size() + 1;
	}

	void VulkanRenderer::CreateInstance()
	{
		vulkanInstance_ = eastl::make_unique<VulkanInstance>(enableValidationLayers, DebugLevel::LEVEL_WARNINGS);
	}

	void VulkanRenderer::FindPhysicalDevice()
	{
		requiredFeatures = {};
		requiredFeatures.depthBiasClamp = 1.f;
		requiredFeatures.depthBounds = 1.f;
		requiredFeatures.depthClamp = 1.f;
		requiredFeatures.fillModeNonSolid = 1.f;
		requiredFeatures.samplerAnisotropy = 1.f;
		requiredFeatures.wideLines = 1.f;
		requiredFeatures.largePoints = 1.f;
		requiredFeatures.variableMultisampleRate = 1.f;
		requiredFeatures.geometryShader = 1.f;


		VulkanDeviceQueueRequestedFamilies_t requestedQueueFamilies = {};
		requestedQueueFamilies.graphics = true;
		requestedQueueFamilies.compute = true;
		requestedQueueFamilies.present = true;

		vulkanPhysicalDevice_ = eastl::unique_ptr<VulkanPhysicalDevice>(VulkanPhysicalDevice::GetBestPhysicalDevice(
			vulkanInstance_.get(), surface, { VK_KHR_SWAPCHAIN_EXTENSION_NAME }, requiredFeatures,
			{}, requestedQueueFamilies, true));
	}

	void VulkanRenderer::CreateLogicalDevice()
	{
		vulkanLogicalDevice_ = eastl::unique_ptr<VulkanLogicalDevice>(new VulkanLogicalDevice(
			vulkanPhysicalDevice_.get(),
			enableValidationLayers,
			{ VK_KHR_SWAPCHAIN_EXTENSION_NAME },
			requiredFeatures));
		int x = 8 + 8;
	}

	void VulkanRenderer::CreateSwapChain()
	{
		VulkanSwapChainSupportDetails_t details = vulkanPhysicalDevice_->GetSwapChainSupportDetails();

		VkSurfaceFormatKHR surfaceFormat = vulkanPhysicalDevice_->ChooseSwapSurfaceFormat(details.formats);
		VkPresentModeKHR presentMode = vulkanPhysicalDevice_->ChooseSwapPresentMode(details.presentModes);
		VkExtent2D extent = vulkanPhysicalDevice_->ChooseSwapExtent(details.capabilities, window.lock()->GetDisplayWidth(), window.lock()->GetDisplayHeight());

		uint32_t imageCount = details.capabilities.minImageCount + 1;
		if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
			imageCount = details.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VulkanDeviceQueueFamilies_t queueFamilies = vulkanPhysicalDevice_->GetQueueFamilies();
		uint32_t queueFamilyIndices[] = { (uint32_t)queueFamilies.graphics, (uint32_t)queueFamilies.present };

		if (queueFamilies.graphics != queueFamilies.present) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult res = vkCreateSwapchainKHR(vulkanLogicalDevice_->GetDevice(), &createInfo, nullptr, &swapChain);

		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create new swapchain";
			std::cout << s.c_str() << std::endl;
		}

		swapChainImageFormat = surfaceFormat.format;
		swapChainImageExtent = extent;
	}

	void VulkanRenderer::GetSwapChainImages()
	{
		imageCount = 0;
		vkGetSwapchainImagesKHR(vulkanLogicalDevice_->GetDevice(), swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(vulkanLogicalDevice_->GetDevice(), swapChain, &imageCount, swapChainImages.data());
	}

	void VulkanRenderer::CreateSwapChainImageViews()
	{
		swapChainImageViews.resize(swapChainImages.size());
		for (int i = 0; i < static_cast<int>(swapChainImages.size()); ++i) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkResult res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &createInfo, nullptr, &swapChainImageViews[i]);

			if (res != VK_SUCCESS) {
				eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Image view creation creation failed";
				std::cout << s.c_str() << std::endl;
			}
		}
	}

	void VulkanRenderer::CreateGraphicsCommandPool()
	{
		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = vulkanPhysicalDevice_->GetQueueFamilies().graphics;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkResult res = vkCreateCommandPool(vulkanLogicalDevice_->GetDevice(), &createInfo, nullptr, &graphicsCommandPool);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Graphics command pool creation failed";
			std::cout << s.c_str() << std::endl;
		}
	}

	void VulkanRenderer::CreateComputeCommandPool()
	{
		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = vulkanPhysicalDevice_->GetQueueFamilies().compute;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkResult res = vkCreateCommandPool(vulkanLogicalDevice_->GetDevice(), &createInfo, nullptr, &computeCommandPool);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Graphics command pool creation failed";
			std::cout << s.c_str() << std::endl;
		}
	}

	void VulkanRenderer::CreateVmaAllocator()
	{
		VmaAllocatorCreateInfo createInfo = {};
		createInfo.physicalDevice = vulkanPhysicalDevice_->GetPhysicalDevice();
		createInfo.device = vulkanLogicalDevice_->GetDevice();
		createInfo.preferredLargeHeapBlockSize = 64 * 1024 * 1024;
		createInfo.pAllocationCallbacks = nullptr;
		createInfo.pDeviceMemoryCallbacks = nullptr;
		createInfo.frameInUseCount = imageCount;
		createInfo.pHeapSizeLimit = nullptr;
		createInfo.pVulkanFunctions = nullptr;

		VkResult res = vmaCreateAllocator(&createInfo, &vmaAllocator_);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create vulkan memory allocator";
			std::cout << s.c_str() << std::endl;
		}
	}

	void VulkanRenderer::CreateDepthImage()
	{
		VkFormat depthFormat = FindSupportedDepthFormat({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		this->depthImageFormat = depthFormat;

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = swapChainImageExtent.width;
		imageCreateInfo.extent.height = swapChainImageExtent.height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = depthFormat;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkResult res = vmaCreateImage(vmaAllocator_, &imageCreateInfo, &allocInfo, &depthImage, &depthImageAllocation, &depthImageAllocationInfo);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create depth image";
			std::cout << s.c_str() << std::endl;
		}

		VkImageViewCreateInfo viewCreateInfo = {};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = depthImage;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = depthFormat;
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;
		viewCreateInfo.subresourceRange.levelCount = 1;

		res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &viewCreateInfo, nullptr, &depthImageView);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create depth image view";
			std::cout << s.c_str() << std::endl;
		}

		VkCommandBufferAllocateInfo commandBufferInfo = {};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandPool = graphicsCommandPool;
		commandBufferInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(vulkanLogicalDevice_->GetDevice(), &commandBufferInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = depthImage;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		if (depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || depthFormat == VK_FORMAT_D24_UNORM_S8_UINT)
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(vulkanLogicalDevice_->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vulkanLogicalDevice_->GetGraphicsQueue());
		vkFreeCommandBuffers(vulkanLogicalDevice_->GetDevice(), graphicsCommandPool, 1, &commandBuffer);

	}

	void VulkanRenderer::CreateAttachmentImages()
	{
		VkImageCreateInfo imguiImageCreateInfo = {};
		imguiImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imguiImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imguiImageCreateInfo.extent.width = swapChainImageExtent.width;
		imguiImageCreateInfo.extent.height = swapChainImageExtent.height;
		imguiImageCreateInfo.extent.depth = 1;
		imguiImageCreateInfo.mipLevels = 1;
		imguiImageCreateInfo.arrayLayers = 1;
		imguiImageCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		imguiImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imguiImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imguiImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		imguiImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imguiImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkResult res = vmaCreateImage(vmaAllocator_, &imguiImageCreateInfo, &allocInfo, &imguiImage, &imguiImageAllocation, &imguiImageAllocationInfo);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create imgui image";
			std::cout << s.c_str() << std::endl;
		}

		VkImageViewCreateInfo imguiViewCreateInfo = {};
		imguiViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imguiViewCreateInfo.image = imguiImage;
		imguiViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imguiViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		imguiViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imguiViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imguiViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imguiViewCreateInfo.subresourceRange.layerCount = 1;
		imguiViewCreateInfo.subresourceRange.levelCount = 1;

		res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &imguiViewCreateInfo, nullptr, &imguiImageView);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create imgui image view";
			std::cout << s.c_str() << std::endl;
		}

		VkImageCreateInfo renderImageCreateInfo = {};
		renderImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		renderImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		renderImageCreateInfo.extent.width = swapChainImageExtent.width;
		renderImageCreateInfo.extent.height = swapChainImageExtent.height;
		renderImageCreateInfo.extent.depth = 1;
		renderImageCreateInfo.mipLevels = 1;
		renderImageCreateInfo.arrayLayers = 1;
		renderImageCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		renderImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		renderImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		renderImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		renderImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		renderImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		res = vmaCreateImage(vmaAllocator_, &renderImageCreateInfo, &allocInfo, &renderImage, &renderImageAllocation, &renderImageAllocationInfo);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create render image";
			std::cout << s.c_str() << std::endl;
		}

		VkImageViewCreateInfo renderViewCreateInfo = {};
		renderViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		renderViewCreateInfo.image = renderImage;
		renderViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		renderViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		renderViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		renderViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		renderViewCreateInfo.subresourceRange.baseMipLevel = 0;
		renderViewCreateInfo.subresourceRange.layerCount = 1;
		renderViewCreateInfo.subresourceRange.levelCount = 1;

		res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &renderViewCreateInfo, nullptr, &renderImageView);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create render image view";
			std::cout << s.c_str() << std::endl;
		}

		VkImageCreateInfo albedoImageCreateInfo = {};
		albedoImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		albedoImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		albedoImageCreateInfo.extent.width = swapChainImageExtent.width;
		albedoImageCreateInfo.extent.height = swapChainImageExtent.height;
		albedoImageCreateInfo.extent.depth = 1;
		albedoImageCreateInfo.mipLevels = 1;
		albedoImageCreateInfo.arrayLayers = 1;
		albedoImageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		albedoImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		albedoImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		albedoImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		albedoImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		albedoImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		res = vmaCreateImage(vmaAllocator_, &albedoImageCreateInfo, &allocInfo, &albedoImage, &albedoImageAllocation, &albedoImageAllocationInfo);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create albedo image";
			std::cout << s.c_str() << std::endl;
		}

		VkImageViewCreateInfo albedoViewCreateInfo = {};
		albedoViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		albedoViewCreateInfo.image = albedoImage;
		albedoViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		albedoViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		albedoViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		albedoViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		albedoViewCreateInfo.subresourceRange.baseMipLevel = 0;
		albedoViewCreateInfo.subresourceRange.layerCount = 1;
		albedoViewCreateInfo.subresourceRange.levelCount = 1;

		res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &albedoViewCreateInfo, nullptr, &albedoImageView);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create albedo image view";
			std::cout << s.c_str() << std::endl;
		}

		VkImageCreateInfo positionImageCreateInfo = {};
		positionImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		positionImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		positionImageCreateInfo.extent.width = swapChainImageExtent.width;
		positionImageCreateInfo.extent.height = swapChainImageExtent.height;
		positionImageCreateInfo.extent.depth = 1;
		positionImageCreateInfo.mipLevels = 1;
		positionImageCreateInfo.arrayLayers = 1;
		positionImageCreateInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
		positionImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		positionImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		positionImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		positionImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		positionImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		res = vmaCreateImage(vmaAllocator_, &positionImageCreateInfo, &allocInfo, &positionImage, &positionImageAllocation, &positionImageAllocationInfo);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create position image";
			std::cout << s.c_str() << std::endl;
		}

		VkImageViewCreateInfo positionViewCreateInfo = {};
		positionViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		positionViewCreateInfo.image = positionImage;
		positionViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		positionViewCreateInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
		positionViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		positionViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		positionViewCreateInfo.subresourceRange.baseMipLevel = 0;
		positionViewCreateInfo.subresourceRange.layerCount = 1;
		positionViewCreateInfo.subresourceRange.levelCount = 1;

		res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &positionViewCreateInfo, nullptr, &positionImageView);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create position image view";
			std::cout << s.c_str() << std::endl;
		}

		VkImageCreateInfo normalImageCreateInfo = {};
		normalImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		normalImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		normalImageCreateInfo.extent.width = swapChainImageExtent.width;
		normalImageCreateInfo.extent.height = swapChainImageExtent.height;
		normalImageCreateInfo.extent.depth = 1;
		normalImageCreateInfo.mipLevels = 1;
		normalImageCreateInfo.arrayLayers = 1;
		normalImageCreateInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
		normalImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		normalImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		normalImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		normalImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		normalImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		res = vmaCreateImage(vmaAllocator_, &normalImageCreateInfo, &allocInfo, &normalImage, &normalImageAllocation, &normalImageAllocationInfo);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create normal image";
			std::cout << s.c_str() << std::endl;
		}

		VkImageViewCreateInfo normalViewCreateInfo = {};
		normalViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		normalViewCreateInfo.image = normalImage;
		normalViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		normalViewCreateInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
		normalViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		normalViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		normalViewCreateInfo.subresourceRange.baseMipLevel = 0;
		normalViewCreateInfo.subresourceRange.layerCount = 1;
		normalViewCreateInfo.subresourceRange.levelCount = 1;

		res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &normalViewCreateInfo, nullptr, &normalImageView);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create normal image view";
			std::cout << s.c_str() << std::endl;
		}


		VkImageCreateInfo ssaoImageCreateInfo = {};
		ssaoImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ssaoImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		ssaoImageCreateInfo.extent.width = swapChainImageExtent.width;
		ssaoImageCreateInfo.extent.height = swapChainImageExtent.height;
		ssaoImageCreateInfo.extent.depth = 1;
		ssaoImageCreateInfo.mipLevels = 1;
		ssaoImageCreateInfo.arrayLayers = 1;
		ssaoImageCreateInfo.format = VK_FORMAT_R16_UNORM;
		ssaoImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		ssaoImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ssaoImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		ssaoImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		ssaoImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		res = vmaCreateImage(vmaAllocator_, &ssaoImageCreateInfo, &allocInfo, &ssaoImage, &ssaoImageAllocation, &ssaoImageAllocationInfo);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create ssao image";
			std::cout << s.c_str() << std::endl;
		}

		VkImageViewCreateInfo ssaoViewCreateInfo = {};
		ssaoViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ssaoViewCreateInfo.image = ssaoImage;
		ssaoViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ssaoViewCreateInfo.format = VK_FORMAT_R16_UNORM;
		ssaoViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ssaoViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ssaoViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ssaoViewCreateInfo.subresourceRange.layerCount = 1;
		ssaoViewCreateInfo.subresourceRange.levelCount = 1;

		res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &ssaoViewCreateInfo, nullptr, &ssaoImageView);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create ssao image view";
			std::cout << s.c_str() << std::endl;
		}

		VkImageCreateInfo ssaoBlurImageCreateInfo = {};
		ssaoBlurImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ssaoBlurImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		ssaoBlurImageCreateInfo.extent.width = swapChainImageExtent.width;
		ssaoBlurImageCreateInfo.extent.height = swapChainImageExtent.height;
		ssaoBlurImageCreateInfo.extent.depth = 1;
		ssaoBlurImageCreateInfo.mipLevels = 1;
		ssaoBlurImageCreateInfo.arrayLayers = 1;
		ssaoBlurImageCreateInfo.format = VK_FORMAT_R16_UNORM;
		ssaoBlurImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		ssaoBlurImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ssaoBlurImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		ssaoBlurImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		ssaoBlurImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		res = vmaCreateImage(vmaAllocator_, &ssaoBlurImageCreateInfo, &allocInfo, &ssaoBlurImage, &ssaoBlurImageAllocation, &ssaoBlurImageAllocationInfo);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create ssaoBlur image";
			std::cout << s.c_str() << std::endl;
		}

		VkImageViewCreateInfo ssaoBlurViewCreateInfo = {};
		ssaoBlurViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ssaoBlurViewCreateInfo.image = ssaoBlurImage;
		ssaoBlurViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ssaoBlurViewCreateInfo.format = VK_FORMAT_R16_UNORM;
		ssaoBlurViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ssaoBlurViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ssaoBlurViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ssaoBlurViewCreateInfo.subresourceRange.layerCount = 1;
		ssaoBlurViewCreateInfo.subresourceRange.levelCount = 1;

		res = vkCreateImageView(vulkanLogicalDevice_->GetDevice(), &ssaoBlurViewCreateInfo, nullptr, &ssaoBlurImageView);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Failed to create ssaoBlur image view";
			std::cout << s.c_str() << std::endl;
		}

	}

	void VulkanRenderer::CreateRenderPass()
	{

		gBufferPassAttachments.resize(static_cast<size_t>(GBufferAttachments::ATTACHMENT_COUNT));

		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].flags = 0;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].format = depthImageFormat;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].flags = 0;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].format = VK_FORMAT_R8G8B8A8_UNORM;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].flags = 0;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].flags = 0;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		gBufferPassAttachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;



		ssaoPassAttachments.resize(static_cast<size_t>(SsaoAttachments::ATTACHMENT_COUNT));

		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].flags = 0;
		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].format = VK_FORMAT_R16_UNORM;
		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ssaoPassAttachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		renderPassAttachments.resize(static_cast<int>(RenderPassAttachments::ATTACHMENT_COUNT));

		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].flags = 0;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].format = VK_FORMAT_B8G8R8A8_UNORM;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].flags = 0;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].format = depthImageFormat;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].flags = 0;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].format = VK_FORMAT_R16_UNORM;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].flags = 0;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].format = VK_FORMAT_R8G8B8A8_UNORM;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].flags = 0;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].flags = 0;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].flags = 0;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].format = VK_FORMAT_B8G8R8A8_UNORM;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].flags = 0;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].format = VK_FORMAT_B8G8R8A8_UNORM;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].samples = VK_SAMPLE_COUNT_1_BIT;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		renderPassAttachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


		gBufferPassSubPasses.resize(static_cast<int>(GBufferSubPasses::SUBPASS_COUNT));

		VkAttachmentReference gBufferDepthReference = {};
		gBufferDepthReference.attachment = static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT);
		gBufferDepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference gBufferAttachments[] = {
			{
				static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT),
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			},
			{
				static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT),
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			},
			{
				static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT),
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			}
		};

		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].flags = 0;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].inputAttachmentCount = 0;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].pInputAttachments = nullptr;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].colorAttachmentCount = 3;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].pColorAttachments = gBufferAttachments;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].pResolveAttachments = nullptr;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].pDepthStencilAttachment = &gBufferDepthReference;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].preserveAttachmentCount = 0;
		gBufferPassSubPasses[static_cast<int>(GBufferSubPasses::G_BUFFER_PASS)].pPreserveAttachments = nullptr;

		eastl::vector<VkSubpassDependency> gbufferDependencies = {
			{
				VK_SUBPASS_EXTERNAL,
				static_cast<int>(GBufferSubPasses::G_BUFFER_PASS),
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
			},
			{
				static_cast<int>(GBufferSubPasses::G_BUFFER_PASS),
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_DEPENDENCY_BY_REGION_BIT
			}
		};

		VkRenderPassCreateInfo gbufferPassCreateInfo = {};
		gbufferPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		gbufferPassCreateInfo.attachmentCount = static_cast<uint32_t>(gBufferPassAttachments.size());
		gbufferPassCreateInfo.pAttachments = gBufferPassAttachments.data();
		gbufferPassCreateInfo.dependencyCount = static_cast<uint32_t>(gbufferDependencies.size());
		gbufferPassCreateInfo.pDependencies = gbufferDependencies.data();
		gbufferPassCreateInfo.subpassCount = static_cast<uint32_t>(gBufferPassSubPasses.size());
		gbufferPassCreateInfo.pSubpasses = gBufferPassSubPasses.data();
		gbufferPassCreateInfo.flags = 0;

		vkCreateRenderPass(vulkanLogicalDevice_->GetDevice(), &gbufferPassCreateInfo, nullptr, &gBufferPass);

		ssaoPassSubPasses.resize(static_cast<int>(SsaoSubPasses::SUBPASS_COUNT));

		VkAttachmentReference ssaoReference = {};
		ssaoReference.attachment = static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT);
		ssaoReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].flags = 0;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].inputAttachmentCount = 0;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].pInputAttachments = nullptr;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].colorAttachmentCount = 1;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].pColorAttachments = &ssaoReference;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].pResolveAttachments = nullptr;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].pDepthStencilAttachment = nullptr;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].preserveAttachmentCount = 0;
		ssaoPassSubPasses[static_cast<int>(SsaoSubPasses::SSAO_PASS)].pPreserveAttachments = nullptr;

		eastl::vector<VkSubpassDependency> ssaoDependencies = {
			{
				VK_SUBPASS_EXTERNAL,
				static_cast<int>(SsaoSubPasses::SSAO_PASS),
				VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_DEPENDENCY_BY_REGION_BIT
			},
			{
				static_cast<int>(SsaoSubPasses::SSAO_PASS),
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_DEPENDENCY_BY_REGION_BIT
			}
		};

		VkRenderPassCreateInfo ssaoPassCreateInfo = {};
		ssaoPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		ssaoPassCreateInfo.attachmentCount = static_cast<uint32_t>(ssaoPassAttachments.size());
		ssaoPassCreateInfo.pAttachments = ssaoPassAttachments.data();
		ssaoPassCreateInfo.dependencyCount = static_cast<uint32_t>(ssaoDependencies.size());
		ssaoPassCreateInfo.pDependencies = ssaoDependencies.data();
		ssaoPassCreateInfo.subpassCount = static_cast<uint32_t>(ssaoPassSubPasses.size());
		ssaoPassCreateInfo.pSubpasses = ssaoPassSubPasses.data();
		ssaoPassCreateInfo.flags = 0;

		vkCreateRenderPass(vulkanLogicalDevice_->GetDevice(), &ssaoPassCreateInfo, nullptr, &ssaoPass);

		renderPassSubPasses.resize(static_cast<int>(RenderSubPasses::SUBPASS_COUNT));

		VkAttachmentReference ssaoBlurReference = {};
		ssaoBlurReference.attachment = static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT);
		ssaoBlurReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].flags = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].inputAttachmentCount = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].pInputAttachments = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].colorAttachmentCount = 1;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].pColorAttachments = &ssaoBlurReference;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].pResolveAttachments = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].pDepthStencilAttachment = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].preserveAttachmentCount = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS)].pPreserveAttachments = nullptr;

		VkAttachmentReference renderDepthReference = {};
		renderDepthReference.attachment = static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT);
		renderDepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		VkAttachmentReference gBufferInputs[] = {
			{
				static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			},
			{
				static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			},
			{
				static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			}
		};

		VkAttachmentReference renderReference = {};
		renderReference.attachment = static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT);
		renderReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].flags = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].inputAttachmentCount = 3;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].pInputAttachments = gBufferInputs;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].colorAttachmentCount = 1;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].pColorAttachments = &renderReference;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].pResolveAttachments = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].pDepthStencilAttachment = &renderDepthReference;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].preserveAttachmentCount = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::RENDER_PASS)].pPreserveAttachments = nullptr;

		VkAttachmentReference imguiReference = {};
		imguiReference.attachment = static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT);
		imguiReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].flags = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].inputAttachmentCount = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].pInputAttachments = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].colorAttachmentCount = 1;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].pColorAttachments = &imguiReference;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].pResolveAttachments = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].pDepthStencilAttachment = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].preserveAttachmentCount = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::IMGUI_PASS)].pPreserveAttachments = nullptr;

		VkAttachmentReference compositeInputs[] = {
			{
				static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			},

			{
				static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			}
		};

		VkAttachmentReference compositeReference = {};
		compositeReference.attachment = static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT);
		compositeReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].flags = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].inputAttachmentCount = 2;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].pInputAttachments = compositeInputs;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].colorAttachmentCount = 1;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].pColorAttachments = &compositeReference;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].pResolveAttachments = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].pDepthStencilAttachment = nullptr;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].preserveAttachmentCount = 0;
		renderPassSubPasses[static_cast<int>(RenderSubPasses::COMPOSITE_PASS)].pPreserveAttachments = nullptr;



		eastl::vector<VkSubpassDependency> dependecies = {
			{
				VK_SUBPASS_EXTERNAL,
				static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS),
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT
			},

			{
				static_cast<int>(RenderSubPasses::SSAO_BLUR_PASS),
				static_cast<int>(RenderSubPasses::RENDER_PASS),
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT
			},

			{
				static_cast<int>(RenderSubPasses::RENDER_PASS),
				static_cast<int>(RenderSubPasses::COMPOSITE_PASS),
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT
			},

			{
				static_cast<int>(RenderSubPasses::IMGUI_PASS),
				static_cast<int>(RenderSubPasses::COMPOSITE_PASS),
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT
			},

			{
				static_cast<int>(RenderSubPasses::COMPOSITE_PASS),
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT
			}
		};

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.attachmentCount = static_cast<uint32_t>(renderPassAttachments.size());
		createInfo.pAttachments = renderPassAttachments.data();
		createInfo.subpassCount = static_cast<uint32_t>(renderPassSubPasses.size());
		createInfo.pSubpasses = renderPassSubPasses.data();
		createInfo.dependencyCount = static_cast<uint32_t>(dependecies.size());
		createInfo.pDependencies = dependecies.data();

		VkResult res = vkCreateRenderPass(vulkanLogicalDevice_->GetDevice(), &createInfo, nullptr, &renderPass);
	}

	void VulkanRenderer::CreateFramebuffers()
	{
		framebuffers.resize(swapChainImageViews.size());

		for (int i = 0; i < framebuffers.size(); ++i) {
			eastl::vector<VkImageView> attachments(static_cast<int>(RenderPassAttachments::ATTACHMENT_COUNT));
			attachments[static_cast<int>(RenderPassAttachments::COLOR_ATTACHMENT)] = swapChainImageViews[i];
			attachments[static_cast<int>(RenderPassAttachments::DEPTH_ATTACHMENT)] = depthImageView;
			attachments[static_cast<int>(RenderPassAttachments::IMGUI_ATTACHMENT)] = imguiImageView;
			attachments[static_cast<int>(RenderPassAttachments::RENDER_ATTACHMENT)] = renderImageView;
			attachments[static_cast<int>(RenderPassAttachments::ALBEDO_ATTACHMENT)] = albedoImageView;
			attachments[static_cast<int>(RenderPassAttachments::POSITION_ATTACHMENT)] = positionImageView;
			attachments[static_cast<int>(RenderPassAttachments::NORMAL_ATTACHMENT)] = normalImageView;
			attachments[static_cast<int>(RenderPassAttachments::SSAO_BLUR_ATTACHMENT)] = ssaoBlurImageView;

			VkFramebufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass = renderPass;
			createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			createInfo.pAttachments = attachments.data();
			createInfo.width = swapChainImageExtent.width;
			createInfo.height = swapChainImageExtent.height;
			createInfo.layers = 1;

			VkResult res = vkCreateFramebuffer(vulkanLogicalDevice_->GetDevice(), &createInfo, nullptr, &framebuffers[i]);
			if (res != VK_SUCCESS) {
				eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Framebuffer creation failed";
				std::cout << s.c_str() << std::endl;
			}
		}
		eastl::vector<VkImageView> attachments(static_cast<int>(GBufferAttachments::ATTACHMENT_COUNT));
		attachments[static_cast<int>(GBufferAttachments::DEPTH_ATTACHMENT)] = depthImageView;
		attachments[static_cast<int>(GBufferAttachments::ALBEDO_ATTACHMENT)] = albedoImageView;
		attachments[static_cast<int>(GBufferAttachments::POSITION_ATTACHMENT)] = positionImageView;
		attachments[static_cast<int>(GBufferAttachments::NORMAL_ATTACHMENT)] = normalImageView;

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = gBufferPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = swapChainImageExtent.width;
		createInfo.height = swapChainImageExtent.height;
		createInfo.layers = 1;

		VkResult res = vkCreateFramebuffer(vulkanLogicalDevice_->GetDevice(), &createInfo, nullptr, &gBufferFrameBuffer);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Framebuffer creation failed";
			std::cout << s.c_str() << std::endl;
		}

		attachments.resize(static_cast<int>(SsaoAttachments::ATTACHMENT_COUNT));
		attachments[static_cast<int>(SsaoAttachments::SSAO_ATTACHMENT)] = ssaoImageView;

		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = ssaoPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = swapChainImageExtent.width;
		createInfo.height = swapChainImageExtent.height;
		createInfo.layers = 1;

		res = vkCreateFramebuffer(vulkanLogicalDevice_->GetDevice(), &createInfo, nullptr, &ssaoFrameBuffer);
		if (res != VK_SUCCESS) {
			eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Framebuffer creation failed";
			std::cout << s.c_str() << std::endl;
		}
	}

	void VulkanRenderer::GenerateCommandBuffers()
	{
		primaryRenderCommandBuffers_.resize(framebuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = graphicsCommandPool;
		allocInfo.commandBufferCount = static_cast<uint32_t>(primaryRenderCommandBuffers_.size());

		vkAllocateCommandBuffers(vulkanLogicalDevice_->GetDevice(), &allocInfo, primaryRenderCommandBuffers_.data());

		primaryComputeCommandBuffers_.resize(framebuffers.size());

		allocInfo.commandPool = computeCommandPool;
		vkAllocateCommandBuffers(vulkanLogicalDevice_->GetDevice(), &allocInfo, primaryComputeCommandBuffers_.data());

		imguiCommandBuffers_.resize(primaryRenderCommandBuffers_.size());
		spriteRenderCommandBuffers_.resize(primaryRenderCommandBuffers_.size());
		debugRenderCommandBuffers_.resize(primaryRenderCommandBuffers_.size());
		staticMeshCommandBuffers_.resize(primaryRenderCommandBuffers_.size());
		skeletalMeshCommandBuffers_.resize(primaryRenderCommandBuffers_.size());
		clearStencilCommandBuffers_.resize(primaryRenderCommandBuffers_.size());

		staticMeshShadowCommandBuffers_.resize(primaryRenderCommandBuffers_.size());
		skeletalMeshShadowCommandBuffers_.resize(primaryRenderCommandBuffers_.size());
		gBufferShadowCommandBuffers_.resize(primaryRenderCommandBuffers_.size());

		GenerateSecondaryCommandBuffers(imguiCommandBuffers_.data(), static_cast<uint32_t>(imguiCommandBuffers_.size()));

		GenerateSecondaryCommandBuffers(&compositeCommandBuffer_, 1);

		GenerateSecondaryCommandBuffers(&gBufferRenderCommandBuffer_, 1);

		GenerateSecondaryCommandBuffers(spriteRenderCommandBuffers_.data(), static_cast<uint32_t>(spriteRenderCommandBuffers_.size()));

		GenerateSecondaryCommandBuffers(debugRenderCommandBuffers_.data(), static_cast<uint32_t>(debugRenderCommandBuffers_.size()));

		GenerateSecondaryCommandBuffers(clearStencilCommandBuffers_.data(), static_cast<uint32_t>(clearStencilCommandBuffers_.size()));

		GenerateSecondaryCommandBuffers(staticMeshCommandBuffers_.data(), static_cast<uint32_t>(staticMeshCommandBuffers_.size()));

		GenerateSecondaryCommandBuffers(skeletalMeshCommandBuffers_.data(), static_cast<uint32_t>(skeletalMeshCommandBuffers_.size()));

		StartSecondaryCommandBufferRecording(compositeCommandBuffer_,
			VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			renderPass, static_cast<int>(RenderSubPasses::COMPOSITE_PASS), VK_NULL_HANDLE);

		vkCmdBindPipeline(compositeCommandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, compositingPipeline_->GetPipeline());

		VkRect2D scissor = { 0,0,swapChainImageExtent };
		VkViewport viewport = { 0.f,0.f,static_cast<float>(swapChainImageExtent.width),static_cast<float>(swapChainImageExtent.height),-1.f,1.f };

		vkCmdSetScissor(compositeCommandBuffer_, 0, 1, &scissor);

		VkDescriptorSet descriptors[] = { CompositingAttachmentDescriptorSet_ };

		vkCmdBindDescriptorSets(compositeCommandBuffer_,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			compositingPipeline_->GetPipelineLayout(),
			0, 1,
			descriptors,
			0, nullptr);

		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(compositeCommandBuffer_, 0, 1, &vertexBuffer_, offsets);

		vkCmdDraw(compositeCommandBuffer_, 6, 1, 0, 0);

		EndSecondaryCommandBufferRecording(compositeCommandBuffer_);

		StartSecondaryCommandBufferRecording(gBufferRenderCommandBuffer_,
			VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			renderPass, static_cast<int>(RenderSubPasses::RENDER_PASS), VK_NULL_HANDLE);

		vkCmdBindPipeline(gBufferRenderCommandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, gBufferRenderPipeline_->GetPipeline());

		vkCmdSetScissor(gBufferRenderCommandBuffer_, 0, 1, &scissor);

		descriptors[0] = gBufferAttachmentDescriptorSet_;

		vkCmdBindDescriptorSets(gBufferRenderCommandBuffer_,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			gBufferRenderPipeline_->GetPipelineLayout(),
			0, 1,
			descriptors,
			0, nullptr);

		VkDescriptorSet sceneDescriptorSet = GetLightDescriptorSet(0, gBufferRenderPipeline_->GetPipelineId(), 1);
		if (sceneDescriptorSet == VK_NULL_HANDLE) {
			sceneDescriptorSet = CreateLightDescriptorSet(0, gBufferRenderPipeline_->GetPipelineId(), 1, gBufferRenderPipeline_->GetDescriptorSetLayout(1));
		}

		uint32_t dynamicOffset[] = { 0 };

		vkCmdBindDescriptorSets(gBufferRenderCommandBuffer_,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			gBufferRenderPipeline_->GetPipelineLayout(),
			1, 1,
			&sceneDescriptorSet,
			1, dynamicOffset);

		vkCmdBindVertexBuffers(gBufferRenderCommandBuffer_, 0, 1, &vertexBuffer_, offsets);

		vkCmdDraw(gBufferRenderCommandBuffer_, 6, 1, 0, 0);

		EndSecondaryCommandBufferRecording(gBufferRenderCommandBuffer_);

	}

	void VulkanRenderer::CreateDescriptorPool()
	{
		if (vulkanDescriptorPools_.size() == 0)
			vulkanDescriptorPools_.resize(1);
		vulkanDescriptorPools_[0] = eastl::shared_ptr<VulkanDescriptorPool>(new VulkanDescriptorPool(vulkanLogicalDevice_.get()));

		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000);
		vulkanDescriptorPools_[0]->AddPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000);

		vulkanDescriptorPools_[0]->Compile(11 * 1000);
	}

	void VulkanRenderer::InitImGui()
	{
		vkQueueWaitIdle(vulkanLogicalDevice_->GetGraphicsQueue());

		ImGui_ImplGlfwVulkan_Init_Data initData = {};
		initData.allocator = nullptr;
		initData.gpu = vulkanPhysicalDevice_->GetPhysicalDevice();
		initData.device = vulkanLogicalDevice_->GetDevice();
		initData.graphics_queue = vulkanLogicalDevice_->GetGraphicsQueue();
		initData.descriptor_pool = vulkanDescriptorPools_[0]->GetPool();
		initData.pipeline_cache = VK_NULL_HANDLE;
		initData.check_vk_result = check_vk_result;
		initData.render_pass = renderPass;
		initData.sub_pass = static_cast<int>(RenderSubPasses::IMGUI_PASS);
		ImGui_ImplGlfwVulkan_Init(window.lock()->GetGLFWWindow().lock().get(), &initData);

		VkCommandBuffer commandBuffer;

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = graphicsCommandPool;
		allocInfo.commandBufferCount = 1;

		vkAllocateCommandBuffers(vulkanLogicalDevice_->GetDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		ImGui_ImplGlfwVulkan_CreateFontsTexture(commandBuffer);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(vulkanLogicalDevice_->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		vkDeviceWaitIdle(vulkanLogicalDevice_->GetDevice());

		ImGui_ImplGlfwVulkan_InvalidateFontUploadObjects();

	}

	void VulkanRenderer::CreateSemaphores()
	{
		imageAvailableSemaphores_.resize(framebuffers.size());
		renderFinishedSemaphores_.resize(framebuffers.size());
		computeFinishedSemaphores_.resize(framebuffers.size());

		drawingFinishedFences_.resize(framebuffers.size());
		computeFinishedFences_.resize(framebuffers.size());

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		for (size_t i = 0, size = framebuffers.size(); i < size; ++i) {
			VkResult res = vkCreateSemaphore(vulkanLogicalDevice_->GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]);
			if (res != VK_SUCCESS) {
				eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Semaphore 'image available' creation failed";
				debug_error("VulkanRenderer", "CreateSemaphores", s);
			}
			res = vkCreateSemaphore(vulkanLogicalDevice_->GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]);
			if (res != VK_SUCCESS) {
				eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Semaphore 'render finished' creation failed";
				debug_error("VulkanRenderer", "CreateSemaphores", s);
			}
			res = vkCreateSemaphore(vulkanLogicalDevice_->GetDevice(), &semaphoreInfo, nullptr, &computeFinishedSemaphores_[i]);
			if (res != VK_SUCCESS) {
				eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Semaphore 'compute finished' creation failed";
				debug_error("VulkanRenderer", "CreateSemaphores", s);
			}

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			res = vkCreateFence(vulkanLogicalDevice_->GetDevice(), &fenceInfo, nullptr, &drawingFinishedFences_[i]);
			if (res != VK_SUCCESS) {
				eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Fence 'drawing finished' creation failed";
				debug_error("VulkanRenderer", "CreateSemaphores", s);
			}

			res = vkCreateFence(vulkanLogicalDevice_->GetDevice(), &fenceInfo, nullptr, &computeFinishedFences_[i]);
			if (res != VK_SUCCESS) {
				eastl::string s = eastl::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Fence 'compute finished' creation failed";
				debug_error("VulkanRenderer", "CreateSemaphores", s);
			}


		}
	}

	void VulkanRenderer::CreateCompositingPipeline()
	{
		compositingPipeline_ = eastl::unique_ptr<VulkanPipeline>(new VulkanPipeline(vulkanLogicalDevice_.get(), this));

		compositingPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "Composite.vert.spv");
		compositingPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "Composite.frag.spv");

		compositingPipeline_->CreateDescriptorSet();
		compositingPipeline_->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);
		compositingPipeline_->AddDescriptorSetBinding(0, 1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);

		compositingPipeline_->AddVertexInputBindingDescription(0, sizeof(glm::vec4), VK_VERTEX_INPUT_RATE_VERTEX);
		compositingPipeline_->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0);

		compositingPipeline_->SetRasterizerSettings(0, 0, VK_POLYGON_MODE_FILL, 1.f, VK_CULL_MODE_NONE);

		compositingPipeline_->SetRenderPassInfo(renderPass, static_cast<int>(RenderSubPasses::COMPOSITE_PASS));

		compositingPipeline_->CreateColorBlendAttachment();

		compositingPipeline_->Compile();

		VkDescriptorSetLayout layouts[] = { compositingPipeline_->GetDescriptorSetLayout(0) };
		vulkanDescriptorPools_[0]->AllocateDescriptorSet(1, layouts, &CompositingAttachmentDescriptorSet_);

		vulkanDescriptorPools_[0]->DescriptorSetBindToImage(CompositingAttachmentDescriptorSet_, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, renderImageView, VK_NULL_HANDLE, 0, 0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1);
		vulkanDescriptorPools_[0]->DescriptorSetBindToImage(CompositingAttachmentDescriptorSet_, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, imguiImageView, VK_NULL_HANDLE, 1, 0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1);

		VkBufferCreateInfo vertexBufferInfo = {};
		vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vertexBufferInfo.size = sizeof(glm::vec4) * 6;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo vertexBufferAllocInfo = {};
		vertexBufferAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		vmaCreateBuffer(vmaAllocator_, &vertexBufferInfo, &vertexBufferAllocInfo, &vertexBuffer_, &vertexBufferAllocation_, nullptr);

		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		stagingBufferInfo.size = sizeof(glm::vec4) * 6;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		VmaAllocationCreateInfo stagingBufferAllocInfo = {};
		stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferAllocation;
		VmaAllocationInfo stagingBufferAllocationInfo;

		vmaCreateBuffer(vmaAllocator_, &stagingBufferInfo, &stagingBufferAllocInfo, &stagingBuffer, &stagingBufferAllocation, &stagingBufferAllocationInfo);

		void* location;

		vkMapMemory(vulkanLogicalDevice_->GetDevice(), stagingBufferAllocationInfo.deviceMemory, stagingBufferAllocationInfo.offset, stagingBufferAllocationInfo.size, 0, &location);

		eastl::vector<glm::vec4> coords = {
			{ -1.f,1.f, 0.f, 0.f },
			{ 1.f,1.f, 1.f, 0.f },
			{ -1.f,-1.f, 0.f, 1.f },
			{ -1.f,-1.f, 0.f, 1.f },
			{ 1.f,1.f, 1.f, 0.f },
			{ 1.f,-1.f, 1.f, 1.f }
		};

		memcpy(location, coords.data(), sizeof(glm::vec4) * 6);

		vkUnmapMemory(vulkanLogicalDevice_->GetDevice(), stagingBufferAllocationInfo.deviceMemory);

		VkCommandBufferAllocateInfo cmdAllocInfo = {};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.commandPool = graphicsCommandPool;
		cmdAllocInfo.commandBufferCount = 1;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkCommandBuffer cmdBuffer;

		vkAllocateCommandBuffers(vulkanLogicalDevice_->GetDevice(), &cmdAllocInfo, &cmdBuffer);

		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = sizeof(glm::vec4) * 6;
		vkCmdCopyBuffer(cmdBuffer, stagingBuffer, vertexBuffer_, 1, &copyRegion);

		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(vulkanLogicalDevice_->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vulkanLogicalDevice_->GetGraphicsQueue());

		vmaDestroyBuffer(vmaAllocator_, stagingBuffer, stagingBufferAllocation);

	}

	void VulkanRenderer::CreateGBufferRenderPipeline()
	{
		gBufferRenderPipeline_ = eastl::unique_ptr<VulkanPipeline>(new VulkanPipeline(vulkanLogicalDevice_.get(), this));

		gBufferRenderPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "G-BufferRender.vert.spv");
		gBufferRenderPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "G-BufferRender.frag.spv");

		gBufferRenderPipeline_->CreateDescriptorSet();
		gBufferRenderPipeline_->AddDescriptorSetLayout(CreateLightDescriptorSetLayout());

		gBufferRenderPipeline_->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);
		gBufferRenderPipeline_->AddDescriptorSetBinding(0, 1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);
		gBufferRenderPipeline_->AddDescriptorSetBinding(0, 2, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);

		gBufferRenderPipeline_->AddVertexInputBindingDescription(0, sizeof(glm::vec4), VK_VERTEX_INPUT_RATE_VERTEX);
		gBufferRenderPipeline_->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0);

		gBufferRenderPipeline_->SetRasterizerSettings(0, 0, VK_POLYGON_MODE_FILL, 1.f, VK_CULL_MODE_NONE);

		gBufferRenderPipeline_->SetRenderPassInfo(renderPass, static_cast<int>(RenderSubPasses::RENDER_PASS));

		gBufferRenderPipeline_->CreateColorBlendAttachment(VK_TRUE, 15U,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD);

		VkStencilOpState front = {};
		front.failOp = VK_STENCIL_OP_KEEP;
		front.passOp = VK_STENCIL_OP_KEEP;
		front.depthFailOp = VK_STENCIL_OP_KEEP;
		front.compareOp = VK_COMPARE_OP_EQUAL;
		front.reference = 0;
		front.compareMask = 0xff;
		front.writeMask = 0xff;

		VkStencilOpState back = {};
		back.failOp = VK_STENCIL_OP_KEEP;
		back.passOp = VK_STENCIL_OP_KEEP;
		back.depthFailOp = VK_STENCIL_OP_KEEP;
		back.compareOp = VK_COMPARE_OP_EQUAL;
		back.reference = 0;
		back.compareMask = 0xff;
		back.writeMask = 0xff;

		gBufferRenderPipeline_->SetDepthStencilState(VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS, 0U, 0U, 1.f, VK_TRUE, front, back);

		gBufferRenderPipeline_->Compile();

		VkDescriptorSetLayout layouts[] = { gBufferRenderPipeline_->GetDescriptorSetLayout(0) };
		vulkanDescriptorPools_[0]->AllocateDescriptorSet(1, layouts, &gBufferAttachmentDescriptorSet_);

		vulkanDescriptorPools_[0]->DescriptorSetBindToImage(gBufferAttachmentDescriptorSet_, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, albedoImageView, VK_NULL_HANDLE,
			0, 0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1);
		vulkanDescriptorPools_[0]->DescriptorSetBindToImage(gBufferAttachmentDescriptorSet_, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, positionImageView, VK_NULL_HANDLE,
			1, 0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1);
		vulkanDescriptorPools_[0]->DescriptorSetBindToImage(gBufferAttachmentDescriptorSet_, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, normalImageView, VK_NULL_HANDLE,
			2, 0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1);

		sceneDataBuffer = eastl::unique_ptr<VulkanBuffer>(new VulkanBuffer(
			vulkanLogicalDevice_.get(), vmaAllocator_, static_cast<uint32_t>(sizeof(SceneInfo)),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, false, graphicsCommandPool));

		lightBuffer = eastl::unique_ptr<VulkanBuffer>(new VulkanBuffer(
			vulkanLogicalDevice_.get(), vmaAllocator_, static_cast<uint32_t>(sizeof(Light)*lightData.size()),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, false, graphicsCommandPool));

		ambientLightPipeline_ = eastl::unique_ptr<VulkanPipeline>(new VulkanPipeline(vulkanLogicalDevice_.get(), this));

		ambientLightPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::VERTEX_SHADER, "G-BufferRender.vert.spv");
		ambientLightPipeline_->LoadShader(VulkanPipeline::SHADER_TYPE::FRAGMENT_SHADER, "G-BufferRender.frag.spv");

		ambientLightPipeline_->CreateDescriptorSet();
		ambientLightPipeline_->AddDescriptorSetLayout(CreateLightDescriptorSetLayout());

		ambientLightPipeline_->AddDescriptorSetBinding(0, 0, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);
		ambientLightPipeline_->AddDescriptorSetBinding(0, 1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);
		ambientLightPipeline_->AddDescriptorSetBinding(0, 2, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr);

		ambientLightPipeline_->AddVertexInputBindingDescription(0, sizeof(glm::vec4), VK_VERTEX_INPUT_RATE_VERTEX);
		ambientLightPipeline_->AddVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, 0);

		ambientLightPipeline_->SetRasterizerSettings(0, 0, VK_POLYGON_MODE_FILL, 1.f, VK_CULL_MODE_NONE);

		ambientLightPipeline_->SetRenderPassInfo(renderPass, static_cast<int>(RenderSubPasses::RENDER_PASS));

		ambientLightPipeline_->CreateColorBlendAttachment(VK_TRUE, 15U,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE, VK_BLEND_OP_ADD);

		ambientLightPipeline_->SetDepthStencilState(VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS, 0U, 0U, 1.f, VK_FALSE, front, back);

		ambientLightPipeline_->Compile();
	}

	void VulkanRenderer::CreateRenderers()
	{
		vulkanSpriteRenderer = eastl::unique_ptr<VulkanSpriteRenderer>(new VulkanSpriteRenderer(this, vulkanLogicalDevice_.get(), vulkanDescriptorPools_[0].get()));
		vulkanDebugRenderer = eastl::unique_ptr<VulkanDebugRenderer>(new VulkanDebugRenderer(this, vulkanLogicalDevice_.get(), vulkanDescriptorPools_[0].get()));
		vulkanStaticMeshRenderer = eastl::unique_ptr<VulkanStaticMeshRenderer>(new VulkanStaticMeshRenderer(this, vulkanLogicalDevice_.get(), vulkanDescriptorPools_[0].get()));
		vulkanSkeletalMeshRenderer = eastl::unique_ptr<VulkanSkeletalMeshRenderer>(new VulkanSkeletalMeshRenderer(this, vulkanLogicalDevice_.get(), vulkanDescriptorPools_[0].get()));
	}

	void VulkanRenderer::AllocateThreads()
	{
		int threadCount = std::thread::hardware_concurrency() - 1;
		threads.resize(threadCount);
		vulkanDescriptorPools_.resize(threadCount + 1);

		for (size_t i = 0, size = threads.size(); i < size; ++i) {
			threads[i] = new ThreadInfo;

			VkCommandPoolCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.queueFamilyIndex = vulkanPhysicalDevice_->GetQueueFamilies().graphics;

			vkCreateCommandPool(vulkanLogicalDevice_->GetDevice(), &info, nullptr, &(threads[i]->commandPool));

			vulkanDescriptorPools_[i + 1] = eastl::shared_ptr<VulkanDescriptorPool>(new VulkanDescriptorPool(vulkanLogicalDevice_.get()));

			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000);
			vulkanDescriptorPools_[i + 1]->AddPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000);

			vulkanDescriptorPools_[i + 1]->Compile(11 * 1000);

			threads[i]->threadId = i + 1;
		}
	}

	void VulkanRenderer::DestroyInstance()
	{
		vkDestroySurfaceKHR(vulkanInstance_->GetInstance(), surface, nullptr);
		vulkanInstance_.reset();
	}

	void VulkanRenderer::DestroyLogicalDevice()
	{
		vulkanLogicalDevice_.reset();
	}

	void VulkanRenderer::DestroySwapChain()
	{
		vkDestroySwapchainKHR(vulkanLogicalDevice_->GetDevice(), swapChain, nullptr);
	}

	void VulkanRenderer::DestroySwapChainImageViews()
	{
		for (int i = 0; i < static_cast<int>(swapChainImageViews.size()); ++i) {
			vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), swapChainImageViews[i], nullptr);
		}
	}

	void VulkanRenderer::DestroyGraphicsCommandPool()
	{
		vkDestroyCommandPool(vulkanLogicalDevice_->GetDevice(), graphicsCommandPool, nullptr);
	}

	void VulkanRenderer::DestroyComputeCommandPool()
	{
		vkDestroyCommandPool(vulkanLogicalDevice_->GetDevice(), computeCommandPool, nullptr);
	}

	void VulkanRenderer::DestroyVmaAllocator()
	{
		vmaDestroyAllocator(vmaAllocator_);
	}

	void VulkanRenderer::DestroyDepthImage()
	{
		vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), depthImageView, nullptr);

		vmaDestroyImage(vmaAllocator_, depthImage, depthImageAllocation);
	}

	void VulkanRenderer::DestroyAttachmentImages()
	{
		vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), imguiImageView, nullptr);

		vmaDestroyImage(vmaAllocator_, imguiImage, imguiImageAllocation);

		vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), renderImageView, nullptr);

		vmaDestroyImage(vmaAllocator_, renderImage, renderImageAllocation);

		vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), albedoImageView, nullptr);

		vmaDestroyImage(vmaAllocator_, albedoImage, albedoImageAllocation);

		vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), positionImageView, nullptr);

		vmaDestroyImage(vmaAllocator_, positionImage, positionImageAllocation);

		vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), normalImageView, nullptr);

		vmaDestroyImage(vmaAllocator_, normalImage, normalImageAllocation);

		vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), ssaoImageView, nullptr);

		vmaDestroyImage(vmaAllocator_, ssaoImage, ssaoImageAllocation);

		vkDestroyImageView(vulkanLogicalDevice_->GetDevice(), ssaoBlurImageView, nullptr);

		vmaDestroyImage(vmaAllocator_, ssaoBlurImage, ssaoBlurImageAllocation);

	}

	void VulkanRenderer::DestroyRenderPass()
	{
		vkDestroyRenderPass(vulkanLogicalDevice_->GetDevice(), gBufferPass, nullptr);
		vkDestroyRenderPass(vulkanLogicalDevice_->GetDevice(), ssaoPass, nullptr);
		vkDestroyRenderPass(vulkanLogicalDevice_->GetDevice(), renderPass, nullptr);
	}

	void VulkanRenderer::DestroyFramebuffers()
	{
		for (int i = 0; i < framebuffers.size(); ++i) {
			vkDestroyFramebuffer(vulkanLogicalDevice_->GetDevice(), framebuffers[i], nullptr);
		}
		vkDestroyFramebuffer(vulkanLogicalDevice_->GetDevice(), gBufferFrameBuffer, nullptr);
		vkDestroyFramebuffer(vulkanLogicalDevice_->GetDevice(), ssaoFrameBuffer, nullptr);

	}

	void VulkanRenderer::DestroyDecriptorPool()
	{
		for (size_t i = 0, size = vulkanDescriptorPools_.size(); i < size; ++i) {
			vulkanDescriptorPools_[i].reset();
		}
	}

	void VulkanRenderer::DestroyImGui() const
	{
		ImGui_ImplGlfwVulkan_Shutdown();
	}

	void VulkanRenderer::DestroySemaphores()
	{
		for (size_t i = 0, size = imageAvailableSemaphores_.size(); i < size; ++i) {
			vkDestroySemaphore(vulkanLogicalDevice_->GetDevice(), imageAvailableSemaphores_[i], nullptr);
			vkDestroySemaphore(vulkanLogicalDevice_->GetDevice(), renderFinishedSemaphores_[i], nullptr);
			vkDestroySemaphore(vulkanLogicalDevice_->GetDevice(), computeFinishedSemaphores_[i], nullptr);

			vkDestroyFence(vulkanLogicalDevice_->GetDevice(), drawingFinishedFences_[i], nullptr);
			vkDestroyFence(vulkanLogicalDevice_->GetDevice(), computeFinishedFences_[i], nullptr);
		}

	}

	void VulkanRenderer::DestroyCompositingPipeline()
	{
		compositingPipeline_.reset();
		vmaDestroyBuffer(vmaAllocator_, vertexBuffer_, vertexBufferAllocation_);
	}

	void VulkanRenderer::DestroyGBufferRenderPipeline()
	{
		gBufferRenderPipeline_.reset();
		ambientLightPipeline_.reset();

		sceneDataBuffer.reset();
		lightBuffer.reset();
	}

	void VulkanRenderer::DestroyRenderers()
	{
		vulkanSpriteRenderer.reset();
		vulkanDebugRenderer.reset();
		vulkanStaticMeshRenderer.reset();
		vulkanSkeletalMeshRenderer.reset();
	}

	void VulkanRenderer::DestroyThreads()
	{
		for (size_t i = 0, size = threads.size(); i < size; ++i) {
			if (threads[i]->thread.joinable())
				threads[i]->thread.join();
			vkDestroyCommandPool(vulkanLogicalDevice_->GetDevice(), threads[i]->commandPool, nullptr);
			delete threads[i];
		}
	}

	VkFormat VulkanRenderer::FindSupportedDepthFormat(const eastl::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		if (!vulkanPhysicalDevice_)
			return VK_FORMAT_UNDEFINED;
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(vulkanPhysicalDevice_->GetPhysicalDevice(), format, &props);
			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures&features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures&features) == features) {
				return format;
			}
		}
		return VK_FORMAT_UNDEFINED;
	}

	void VulkanRenderer::GenerateSecondaryCommandBuffers(VkCommandBuffer * buffers, uint32_t count)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocInfo.commandPool = graphicsCommandPool;
		allocInfo.commandBufferCount = count;

		vkAllocateCommandBuffers(vulkanLogicalDevice_->GetDevice(), &allocInfo, buffers);
	}

	void VulkanRenderer::DestroySecondaryCommandBuffers(VkCommandBuffer * buffers, uint32_t count)
	{
		vkFreeCommandBuffers(vulkanLogicalDevice_->GetDevice(), graphicsCommandPool, count, buffers);
	}

	void VulkanRenderer::RestructureLights()
	{
		eastl::array<Light, 1024> tempData = lightData;
		lightData.fill({});
		eastl::map<eastl::string, LightInfo>::iterator it = lights.begin();
		eastl::map<eastl::string, LightInfo>::iterator end = lights.end();
		for (size_t location = 0; it != end; ++it) {
			if (it->second.active) {
				lightData[location] = it->second.light;
				it->second.lightDataIndex = location;
				location++;
				activeLights = static_cast<int>(location);
			}
		}

		lightDataRestructured = false;
		lightDataChanged = true;
	}

	void VulkanRenderer::UpdateLightData()
	{
		lightBuffer->UpdateBuffer(lightData.data(), 0, static_cast<uint32_t>(sizeof(Light)*lightData.size()));

		if (activeLights != scene.lightCount) {
			scene.lightCount = activeLights;
		}

		lightDataChanged = false;
	}

	VulkanRenderer::ThreadInfo * VulkanRenderer::GetFreeThread()
	{
		for (size_t i = 0, size = threads.size(); i < size; ++i) {
			if (threads[i]->initialized == false)
				return threads[i];

			auto status = threads[i]->future.wait_for(std::chrono::milliseconds(0));

			if (status == std::future_status::ready) {
				if (threads[i]->thread.joinable())
					threads[i]->thread.join();
				threads[i]->initialized = false;
				return threads[i];
			}
		}
		return nullptr;
	}

	void VulkanRenderer::JoinAllThreads()
	{
		for (size_t i = 0, size = threads.size(); i < size; ++i) {
			if (threads[i]->thread.joinable())
				threads[i]->thread.join();
		}
	}

	void VulkanRenderer::ResetCommandPools()
	{
		for (size_t i = 0, size = threads.size(); i < size; ++i) {
			vkResetCommandPool(vulkanLogicalDevice_->GetDevice(), threads[i]->commandPool, 0);
		}
	}

	void VulkanRenderer::StartSecondaryCommandBufferRecording(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags, VkRenderPass renderPass, uint32_t subPass, VkFramebuffer framebuffer) const
	{
		VkCommandBufferInheritanceInfo inheritence = {};
		inheritence.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritence.occlusionQueryEnable = VK_FALSE;
		inheritence.renderPass = renderPass;
		inheritence.subpass = subPass;
		inheritence.framebuffer = framebuffer;

		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags = flags;
		info.pInheritanceInfo = &inheritence;

		vkBeginCommandBuffer(buffer, &info);
	}

	void VulkanRenderer::EndSecondaryCommandBufferRecording(VkCommandBuffer buffer) const
	{
		vkEndCommandBuffer(buffer);
	}


} //namespace Engine
#endif