#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include "Engine/Engine/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/VulkanWindow.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Renderer/Vulkan/VulkanInstance.hpp"
#include "Engine/Renderer/Vulkan/VulkanPhysicalDevice.hpp"
#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include "Engine/Renderer/Vulkan/VulkanDescriptorPool.hpp"
#include "Engine/Renderer/Vulkan/VulkanPipeline.hpp"

#include "Engine/Renderer/Vulkan/VulkanSpriteRenderer.hpp"
#include "Engine/Renderer/Vulkan/VulkanDebugRenderer.hpp"
#include "Engine/Renderer/Vulkan/VulkanStaticMeshRenderer.hpp"
#include "Engine/Renderer/Vulkan/VulkanSkeletalMeshRenderer.hpp"

#include "Engine/Texture/Texture.hpp"
#include "Engine/Utility/Vertex.hpp"
#include "Engine/Renderer/Vulkan/vk_mem_alloc.h"
#include "Engine/Renderer/imgui_impl_glfw_vulkan.h"

#include "Engine/Utility/Light.hpp"

// Written by Koen Buitenhuis

//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <future>
#include <thread>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <chrono>



namespace Engine
{

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	/// <summary>
	/// This is the Vulkan renderer. Allows you to render using Vulkan. NOTE: Only the Engine is allowed to create this object.
	/// </summary>
	class ENGINE_API VulkanRenderer : public Renderer {
		friend VulkanPipeline;
		friend std::shared_ptr<Renderer> Engine::GetRenderer() noexcept;

		VulkanRenderer() noexcept;
	public:
		VulkanRenderer(VulkanRenderer const &other) = default;
		VulkanRenderer(VulkanRenderer &&other) noexcept = default;

		virtual ~VulkanRenderer() override;
	private:

	public:
		/// <summary>
		/// Initializes all vulkan objects required by the renderer, as well as sending the nessecary data structures to the texture and mesh classes.
		/// Binds the renderer to the default window, all render calls by this renderer object will only affect this window.
		/// </summary>
		void InitializeRenderer();

		/// <summary>
		/// Initializes all vulkan objects required by the renderer, as well as sending the nessecary data structures to the texture and mesh classes.
		/// Binds the renderer to a specific window, all render calls by this renderer object will only affect this window.
		/// </summary>
		/// <param name="window">The window to bind the renderer to.</param>
		void InitializeRenderer(std::shared_ptr<VulkanWindow> window);

		/// <summary>
		/// Tell the renderer that the window it was bound too was resized, and the pipeline needs to be recreated.
		/// </summary>
		void Resized();

		/// <summary>
		/// Sets the clear color of the background.
		/// </summary>
		/// <param name="color">The clear color for the background.</param>
		void setClearColor(glm::vec4 color);

		/// <summary>
		/// Returns the main vulkan render pass object. Shouldn't be used outside of renderer classes.
		/// </summary>
		/// <returns>the main render pass.</returns>
		VkRenderPass GetRenderPass() const;

		/// <summary>
		/// Returns a vulkan render pass object used for the gbuffer render pass.
		/// </summary>
		/// <returns>The gbuffer render pass.</returns>
		VkRenderPass GetGBufferRenderPass() const;

		/// <summary>
		/// Returns the gpu memory allocator used by the renderer.
		/// </summary>
		/// <returns>the VmaAllocator used by this renderer.</returns>
		VmaAllocator GetVmaAllocator() const;

		/// <summary>
		/// Returns the command pool used by this renderer. Binds to the graphics command queue.
		/// Use this for allocating new command buffers.
		/// </summary>
		/// <returns>the VkCommandPool object used by this allocator.</returns>
		VkCommandPool GetGraphicsCommandPool() const;

		/// <summary>
		/// Returns the command pool used by this renderer. Binds to the compute command queue.
		/// Use this for allocating new command buffers.
		/// </summary>
		/// <returns>the VkCommandPool object used by this allocator.</returns>
		VkCommandPool GetComputeCommandPool() const;

		/// <summary>
		/// Returns the current swap chain image size as a VkExtend2D object. This should be equal to the window size.
		/// </summary>
		/// <returns>A VkExtend2D object with the current swapchain size.</returns>
		VkExtent2D GetSwapChainExtent() const;

		//Should probably add the Shader reference for vertex and fragment to this, so they can be sent to the GPU
		//Possibly also want to add an Entity reference, so you could send it's data (mesh and texture) to the GPU as well.
		//At least that's how it worked in OpenGL
		/// <summary>
		/// Starts the render pass. Call this before rendering anything.
		/// Equal to RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection), but using default values.
		/// </summary>
		virtual void RendererBegin();
		/// <summary>
		/// Starts the render pass. Call this before rendering anything.
		/// </summary>
		/// <param name="view">View matrix to be used.</param>
		/// <param name="projection">Projection matrix to be used</param>
		virtual void RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection);

		//Used to render the selected Shader & Entity combination defined in RendererBegin()
		/// <summary>
		/// Unused.
		/// </summary>
		virtual void Render();

		/// <summary>
		/// Renders a Model. Can currently only render static models.
		/// </summary>
		/// <param name="modelMatrix">Current transform of the model. Applies to all meshes contained by the model.</param>
		/// <param name="model">Model to be rendered.</param>
		/// <param name="mainColor">Color of the model. Should normally be white (glm::vec4(1.f, 1.f, 1.f, 1.f))</param>
		virtual void Render(const glm::mat4x4& modelMatrix, std::shared_ptr<Model> model, const glm::vec4& mainColor = glm::vec4(1.f, 1.f, 1.f, 1.f));

		/// <summary>
		/// Renders a texture in the world. Base size of the texture is a one by one square, center of the texture is the origin.
		/// </summary>
		/// <param name="texture">Texture to be rendered.</param>
		/// <param name="modelMatrix">Transform of the texture.</param>
		virtual void RenderSprite(std::weak_ptr<Texture> texture, glm::mat4 modelMatrix);

		/// <summary>
		/// Renders a line from start coordinates to end coordinates in the specified color.
		/// </summary>
		/// <param name="start">Start coordinates.</param>
		/// <param name="end">End coordinates.</param>
		/// <param name="color">Line Color.</param>
		virtual void RenderLine(glm::vec3 start, glm::vec3 end, glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.f));

		//Used to unbind the current selected Shader & Entity combination defined in RendererBegin()
		/// <summary>
		/// Ends the current render pass and submits the commands to the gpu and presents the resulting image.
		/// Call RendererBegin after this function before rendering anything else.
		/// </summary>
		virtual void RendererEnd();

		/// <summary>
		/// Start the recording of a new secondary command buffer.
		/// </summary>
		/// <param name="buffer">Buffer to start recording in.</param>
		/// <param name="flags">Flags used for the buffer.</param>
		/// <param name="renderPass">The VkRenderPass object the command buffer will be used for.</param>
		/// <param name="subPass">The rendering subpass the command buffer will be used for.</param>
		/// <param name="framebuffer">The VkFramebuffer the current renderpass will be recording in, can be left as VK_NULL_HANLE if unknown</param>
		void StartSecondaryCommandBufferRecording(VkCommandBuffer buffer, VkCommandBufferUsageFlags flags, VkRenderPass renderPass, uint32_t subPass, VkFramebuffer framebuffer) const;

		/// <summary>
		/// Ends secondary command buffer recording. Don't record any new commands into this command buffer after calling this function.
		/// </summary>
		/// <param name="buffer">The command buffer to end recording for.</param>
		void EndSecondaryCommandBufferRecording(VkCommandBuffer buffer) const;

		/// <summary>
		/// Enum with the different subpasses used by the g-buffer renderpass.
		/// Use SUBPASS_COUNT to get the amount of subpasses used by the renderpass.
		/// </summary>
		enum class GBufferSubPasses {
			G_BUFFER_PASS = 0,
			SUBPASS_COUNT
		};

		/// <summary>
		/// Enum with the different subpasses used by the ssao renderpass.
		/// Use SUBPASS_COUNT to get the amount of subpasses used by the renderpass.
		/// </summary>
		enum class SsaoSubPasses {
			SSAO_PASS = 0,
			SUBPASS_COUNT
		};

		/// <summary>
		/// Enum with the different subpasses used by the renderpass.
		/// Use SUBPASS_COUNT to get the amount of subpasses used by the renderpass.
		/// </summary>
		enum class RenderSubPasses {
			SSAO_BLUR_PASS = 0,
			RENDER_PASS,
			IMGUI_PASS,
			COMPOSITE_PASS,
			SUBPASS_COUNT
		};

		/// <summary>
		/// Creates a new light with the given name and type, and initializes it with the given variables.
		/// </summary>
		/// <param name="name">The name of the light. Use this when modifying the light in other functions.</param>
		/// <param name="type">The type of light that's being created.</param>
		/// <param name="position">The position of the light. Unused for directional lights.</param>
		/// <param name="direction">The direction the light points at. Unused for point lights.</param>
		/// <param name="color">The color of the light.</param>
		/// <param name="radius">The radius over which a light has effect.</param>
		/// <param name="attunuation">Attunuation of the light. Currently not used.</param>
		/// <param name="coneInnerAngle">The angle of a spot light in which a pixel recieves the incoming light at full strength.
		/// Only used for spot lights.</param>
		/// <param name="coneOuterAngle">The angle of spot light that is the outer most angle a pixel can still recieve incoming light.
		/// If a pixel is between the inner and outer angle the strength of the light will slowly fade depending how close to the
		/// outer angle the pixel is. Only used for spot lights.</param>
		void CreateLight(std::string name, LightType lightType,
			glm::vec3 position, glm::vec3 direction, glm::vec3 color,
			float radius, float attunuation, float coneInnerAngle, float coneOuterAngle);

		/// <summary>
		/// Changes the type of the light. WARNING: When changing a point light into a spot light, make sure to also set the cone angles.
		/// </summary>
		/// <param name="name">The light to change.</param>
		/// <param name="type">What type the light should become.</param>
		void SetLightType(std::string name, LightType lightType);

		/// <summary>
		/// Returns the type of the light.
		/// </summary>
		/// <param name="name">The light to get the type from.</param>
		/// <returns>The type of the light.</returns>
		LightType GetLightType(std::string name);

		/// <summary>
		/// Changes the position of the light. Does nothing if the light is a directional light.
		/// </summary>
		/// <param name="name">The light to change.</param>
		/// <param name="position">The new position of the light.</param>
		void SetLightPosition(std::string name, glm::vec3 position);

		/// <summary>
		/// Returns the position of the light. If the light can't be found a vector of 0,0,0 is returned.
		/// </summary>
		/// <param name="name">The light whose position to get.</param>
		/// <returns>A glm vec3 containing the position.</returns>
		glm::vec3 GetLightPosition(std::string name);

		/// <summary>
		/// Sets the direction the light shines at. No effect for point lights.
		/// </summary>
		/// <param name="name">The light to change the direction for.</param>
		/// <param name="direction">The new direction.</param>
		void SetLightDirection(std::string name, glm::vec3 direction);

		/// <summary>
		/// Returns the direction of the light. If the light can't be found a vector of 0,0,0 is returned.
		/// </summary>
		/// <param name="name">The light whose direction to get.</param>
		/// <returns>A glm vec3 containing the direction.</returns>
		glm::vec3 GetLightDirection(std::string name);

		/// <summary>
		/// Sets the color of the light.
		/// </summary>
		/// <param name="name">The light to set the color for.</param>
		/// <param name="color">The new color.</param>
		void SetLightColor(std::string name, glm::vec3 color);

		/// <summary>
		/// Returns the color of the light.
		/// </summary>
		/// <param name="name">The light to get the color from.</param>
		/// <returns>A glm vec3 containing the color.</returns>
		glm::vec3 GetLightColor(std::string name);

		/// <summary>
		/// Sets the radius of the light. Only has an effect on point and spot lights.
		/// </summary>
		/// <param name="name">The name of the light.</param>
		/// <param name="radius">The new radius.</param>
		void SetLightRadius(std::string name, float radius);

		/// <summary>
		/// Returns the radius of the light.
		/// </summary>
		/// <param name="name">The name of the light.</param>
		/// <returns></returns>
		float GetLightRadius(std::string name);

		/// <summary>
		/// Sets the inner angle of the light cone. Only affects spot lights.
		/// </summary>
		/// <param name="name">The name of the light.</param>
		/// <param name="angle">The new inner angle in radians.</param>
		void SetLightConeInnerAngle(std::string name, float angle);

		/// <summary>
		/// Returns the inner angle of the light cone. Only affects spot lights.
		/// </summary>
		/// <param name="name">The name of the light.</param>
		/// <returns>the inner angle of the light cone in radians.</returns>
		float GetLightConeInnerAngle(std::string name);

		/// <summary>
		/// Sets the outer angle of the light cone. Only affects spot lights.
		/// </summary>
		/// <param name="name">The name of the light.</param>
		/// <param name="angle">The new outer angle in radians.</param>
		void SetLightConeOuterAngle(std::string name, float angle);

		/// <summary>
		/// Returns the outer angle of the light cone. Only affects spot lights.
		/// </summary>
		/// <param name="name">The name of the light.</param>
		/// <returns>the outer angle of the light cone in radians.</returns>
		float GetLightConeOuterAngle(std::string name);

		/// <summary>
		/// Creates and returns a descriptor set layout matching the light and scene data.
		/// </summary>
		/// <returns></returns>
		VkDescriptorSetLayout CreateLightDescriptorSetLayout();

		/// <summary>
		/// Creates a new discriptor set bound to the renderer's light buffer, and configured for the given pipeline and layout.
		/// </summary>
		/// <param name="pipelineID">The pipeline the discriptor will be bound to.</param>
		/// <param name="set">The descriptor set number this descriptor will be bound to.</param>
		/// <param name="layout">The layout of the descriptor set.</param>
		/// <returns></returns>
		VkDescriptorSet CreateLightDescriptorSet(size_t threadID, size_t pipelineID, size_t set, VkDescriptorSetLayout layout);

		/// <summary>
		/// Returns the light descriptor set bound to the specified pipeline and set number.
		/// </summary>
		/// <param name="pipelineID">The pipeline this descriptor is bound to.</param>
		/// <param name="set">The set of the pipeline this descriptor is bound to.</param>
		/// <returns>The descriptor set bound to the pipeline and set number. 
		/// Returns VK_NULL_HANDLE of no descriptor is bound to the given combination.</returns>
		VkDescriptorSet GetLightDescriptorSet(size_t threadID, size_t pipelineID, size_t set);

		/// <summary>
		/// Returns the framebuffer of the current frame.
		/// </summary>
		/// <returns>The current frame's framebuffer.</returns>
		VkFramebuffer GetFrameBuffer();

		/// <summary>
		/// Returns a descriptor pool for the given thread (thread 0 is the main thread).
		/// Returns a nullptr if the thread id is larger than the number of threads.
		/// </summary>
		/// <param name="thread">The number of the thread the descriptor pool belongs to.</param>
		/// <returns></returns>
		VulkanDescriptorPool* GetDescriptorPool(size_t thread);

		/// <summary>
		/// Returns the number of threads the renderer has. 
		/// This is the main thread + n secondary threads.
		/// </summary>
		/// <returns>The number of threads.</returns>
		size_t GetThreadCount();

	protected:
		void CreateInstance();
		void FindPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void GetSwapChainImages();
		void CreateSwapChainImageViews();
		void CreateGraphicsCommandPool();
		void CreateComputeCommandPool();
		void CreateVmaAllocator();
		void CreateDepthImage();
		void CreateAttachmentImages();
		void CreateRenderPass();
		void CreateFramebuffers();
		void GenerateCommandBuffers();
		void CreateDescriptorPool();
		void InitImGui();
		void CreateSemaphores();
		void CreateCompositingPipeline();
		void CreateGBufferRenderPipeline();
		void CreateSsaoPipelines();
		void CreateRenderers();
		void AllocateThreads();

		void RecreateSwapChain();

		void DestroyInstance();
		void DestroyLogicalDevice();
		void DestroySwapChain();
		void DestroySwapChainImageViews();
		void DestroyGraphicsCommandPool();
		void DestroyComputeCommandPool();
		void DestroyVmaAllocator();
		void DestroyDepthImage();
		void DestroyAttachmentImages();
		void DestroyRenderPass();
		void DestroyFramebuffers();
		void DestroyDecriptorPool();
		void DestroySemaphores();
		void DestroyCompositingPipeline();
		void DestroyGBufferRenderPipeline();
		void DestroySsaoPipelines();
		void DestroyRenderers();
		void DestroyThreads();

		void HandleOnWindowResizedEvent(GLFWwindow* glfwWindow, const int width, const int height);

		void HandleOnWindowShutdownRequestedEvent(const std::shared_ptr<Window> windowPtr);

		VkFormat VulkanRenderer::FindSupportedDepthFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		std::unique_ptr<VulkanInstance> vulkanInstance_;
		std::unique_ptr<VulkanPhysicalDevice> vulkanPhysicalDevice_;
		std::unique_ptr<VulkanLogicalDevice> vulkanLogicalDevice_;
		std::vector<std::shared_ptr<VulkanDescriptorPool>> vulkanDescriptorPools_;

		/*
		VulkanInstance* vulkanInstance;
		VulkanPhysicalDevice* vulkanPhysicalDevice;
		VulkanLogicalDevice* vulkanLogicalDevice;
		VulkanDescriptorPool* vulkanDescriptorPool;*/

		VulkanDeviceFeatures_t requiredFeatures{};

		std::shared_ptr<VulkanWindow> window;

		VkSurfaceKHR surface{};



#pragma region Swapchain

		VkSwapchainKHR swapChain{};

		uint32_t imageCount{};
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainImageExtent{};

		uint32_t currentImage{};
		uint32_t prevImage{};

		VkCommandPool graphicsCommandPool{};
		VkCommandPool computeCommandPool{};

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

#pragma endregion

#pragma region Depth image

		VkImage depthImage{};
		VkImageView depthImageView{};
		VmaAllocation depthImageAllocation{};
		VmaAllocationInfo depthImageAllocationInfo{};

		VkFormat depthImageFormat;

#pragma endregion

#pragma region Renderpass

		enum class GBufferAttachments {
			ALBEDO_ATTACHMENT,
			POSITION_ATTACHMENT,
			NORMAL_ATTACHMENT,
			DEPTH_ATTACHMENT,
			ATTACHMENT_COUNT
		};

		enum class SsaoAttachments {
			SSAO_ATTACHMENT,
			ATTACHMENT_COUNT
		};

		enum class RenderPassAttachments {
			COLOR_ATTACHMENT = 0,
			ALBEDO_ATTACHMENT,
			POSITION_ATTACHMENT,
			NORMAL_ATTACHMENT,
			SSAO_BLUR_ATTACHMENT,
			RENDER_ATTACHMENT,
			IMGUI_ATTACHMENT,
			DEPTH_ATTACHMENT,
			ATTACHMENT_COUNT
		};


		std::vector<VkAttachmentDescription> gBufferPassAttachments;
		std::vector<VkSubpassDescription> gBufferPassSubPasses;

		std::vector<VkAttachmentDescription> ssaoPassAttachments;
		std::vector<VkSubpassDescription> ssaoPassSubPasses;

		std::vector<VkAttachmentDescription> renderPassAttachments;
		std::vector<VkSubpassDescription> renderPassSubPasses;

		VkRenderPass gBufferPass{};
		VkRenderPass ssaoPass{};
		VkRenderPass renderPass{};

#pragma endregion

#pragma region Attachment images

		VkImage imguiImage{};
		VkImageView imguiImageView{};
		VmaAllocation imguiImageAllocation{};
		VmaAllocationInfo imguiImageAllocationInfo{};

		VkImage renderImage{};
		VkImageView renderImageView{};
		VmaAllocation renderImageAllocation{};
		VmaAllocationInfo renderImageAllocationInfo{};

		VkImage albedoImage{};
		VkImageView albedoImageView{};
		VmaAllocation albedoImageAllocation{};
		VmaAllocationInfo albedoImageAllocationInfo{};

		VkImage positionImage{};
		VkImageView positionImageView{};
		VmaAllocation positionImageAllocation{};
		VmaAllocationInfo positionImageAllocationInfo{};

		VkImage normalImage{};
		VkImageView normalImageView{};
		VmaAllocation normalImageAllocation{};
		VmaAllocationInfo normalImageAllocationInfo{};

		VkImage ssaoImage{};
		VkImageView ssaoImageView{};
		VmaAllocation ssaoImageAllocation{};
		VmaAllocationInfo ssaoImageAllocationInfo{};

		VkImage ssaoBlurImage{};
		VkImageView ssaoBlurImageView{};
		VmaAllocation ssaoBlurImageAllocation{};
		VmaAllocationInfo ssaoBlurImageAllocationInfo{};

		VkSampler sampler{};

#pragma endregion

#pragma region Framebuffers

		std::vector<VkFramebuffer> framebuffers;

		VkFramebuffer gBufferFrameBuffer{};
		VkFramebuffer ssaoFrameBuffer{};

#pragma endregion

#pragma region CommandBuffers

		std::vector<VkCommandBuffer> primaryRenderCommandBuffers_;
		std::vector<VkCommandBuffer> primaryComputeCommandBuffers_;

		std::vector<std::vector<VkCommandBuffer>> staticMeshShadowCommandBuffers_;
		std::vector<std::vector<VkCommandBuffer>> skeletalMeshShadowCommandBuffers_;
		std::vector<std::vector<VkCommandBuffer>> gBufferShadowCommandBuffers_;

		VkCommandBuffer compositeCommandBuffer_{};

		VkCommandBuffer gBufferRenderCommandBuffer_{};

		std::vector<VkCommandBuffer> imguiCommandBuffers_;

		VkCommandBuffer currentBuffer_{};
		VkCommandBuffer currentComputeBuffer_{};

		std::vector<VkCommandBuffer> spriteRenderCommandBuffers_;

		std::vector<VkCommandBuffer> debugRenderCommandBuffers_;

		std::vector<VkCommandBuffer> staticMeshCommandBuffers_;

		std::vector<VkCommandBuffer> skeletalMeshCommandBuffers_;

		std::vector<VkCommandBuffer> clearStencilCommandBuffers_;

		void GenerateSecondaryCommandBuffers(VkCommandBuffer *buffers, uint32_t count);
		void DestroySecondaryCommandBuffers(VkCommandBuffer*buffers, uint32_t count);

#pragma endregion

#pragma region Pipelines

		std::unique_ptr<VulkanPipeline> compositingPipeline_;

		std::unique_ptr<VulkanPipeline> gBufferRenderPipeline_;

		std::unique_ptr<VulkanPipeline> ambientLightPipeline_;

		std::unique_ptr<VulkanPipeline> ssaoPipeline_;
		std::unique_ptr<VulkanPipeline> ssaoBlurPipeline_;

		VkDescriptorSet gBufferAttachmentDescriptorSet_{};

		VkDescriptorSet SceneInfoDescriptorSet_{};

		VkDescriptorSet CompositingAttachmentDescriptorSet_{};

		VkDescriptorSet ssaoSamplerDescriptor_{};

		VkDescriptorSet projectionDescriptor_{};

		VkBuffer vertexBuffer_{};
		VmaAllocation vertexBufferAllocation_{};

		std::unique_ptr<VulkanBuffer> ssaoKernelBuffer_;
		std::unique_ptr<VulkanBuffer> projectionBuffer_;

		glm::mat4 projection_;

#pragma endregion

#pragma region Renderers

		std::unique_ptr<VulkanSpriteRenderer> vulkanSpriteRenderer;
		std::unique_ptr<VulkanDebugRenderer> vulkanDebugRenderer;
		std::unique_ptr<VulkanStaticMeshRenderer> vulkanStaticMeshRenderer;
		std::unique_ptr<VulkanSkeletalMeshRenderer> vulkanSkeletalMeshRenderer;

#pragma endregion

#pragma region Scene

		std::unique_ptr<VulkanBuffer> sceneDataBuffer;
		std::unique_ptr<VulkanBuffer> lightBuffer;

		std::array<Light, 1024> lightData;

		typedef struct {
			int lightCount;
			glm::vec4 viewPos;
		}SceneInfo;

		int activeLights{};

		SceneInfo scene;

		typedef struct {
			size_t lightDataIndex;
			Light light;
			std::string name;
			LightType type;
			bool active;
		}LightInfo;

		std::map<std::string, LightInfo> lights;

		std::vector<std::vector<std::vector<VkDescriptorSet>>> lightDescriptorSets_;

		bool lightDataChanged{};
		bool lightDataRestructured{};

		void RestructureLights();

		void UpdateLightData();

#pragma endregion

#pragma region Threads

		struct ThreadInfo {
			std::thread thread;
			std::packaged_task<void()> task;
			std::future<void> future;
			bool initialized = false;
			size_t threadId;
			VkCommandPool commandPool;
		};

		std::vector<ThreadInfo*> threads;

		ThreadInfo* GetFreeThread();
		void JoinAllThreads();
		void ResetCommandPools();

#pragma endregion

		VmaAllocator vmaAllocator_{};

		std::vector<VkSemaphore> imageAvailableSemaphores_;
		std::vector<VkSemaphore> renderFinishedSemaphores_;
		std::vector<VkSemaphore> computeFinishedSemaphores_;

		std::vector<VkFence> drawingFinishedFences_;
		std::vector<VkFence> computeFinishedFences_;

		VkClearValue clearValue{};

		bool initialized;

		bool firstRender{};

		bool resized{};

	};
} // namespace Engine
#endif // USING_VULKAN
