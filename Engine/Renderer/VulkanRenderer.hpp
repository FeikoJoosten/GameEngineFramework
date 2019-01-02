// Written by Koen Buitenhuis

#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/VulkanWindow.hpp"
#include "Engine/Utility/Vertex.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ThirdParty/EASTL-master/include/EASTL/array.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

const int WIDTH = 800;
const int HEIGHT = 600;

namespace Engine
{

const eastl::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const eastl::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VkVertexInputBindingDescription GetBindingDescription() {
	VkVertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

static eastl::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
	eastl::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, position);
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, normal);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoords);

	return attributeDescriptions;
}


struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class ENGINE_API VulkanRenderer : public Renderer {
public:

	VulkanRenderer() noexcept;
	VulkanRenderer(VulkanRenderer const &other) = default;
	VulkanRenderer(VulkanRenderer &&other) noexcept = default;
	~VulkanRenderer() noexcept = default;

	void InitializeRenderer();
	void CleanupRenderer();
	void Render() override; // function exclusive to private renderer.
	void RecreateSwapChain();
	void CreateModel(eastl::vector<Vertex> theVertices, eastl::vector<unsigned int> theIndices, unsigned int VBO, unsigned int EBO, unsigned int UBO);

	void RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection) override;
	void Render(const glm::mat4x4& modelMatrix, Model* model, const glm::vec4& mainColor) override;
	void RendererEnd() override;

private:
	void InitVulkan();

	//Class members
	VkInstance instance;
	VkDebugReportCallbackEXT callback;

	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	eastl::vector<VkPipeline> graphicsPipelines;
	//--

	eastl::vector<VkBuffer*> vertexBuffers;
	eastl::vector<VkBuffer*> indexBuffers;
	eastl::vector<VkBuffer*> uniformBuffers;
	eastl::vector<VkDeviceMemory*> uniformBuffersMemory;

	//Every 3D model needs:
	//VkBuffer m_VertexBuffer;
	//VkDeviceMemory m_VertexBufferMemory;
	//VkBuffer m_IndexBuffer;
	//VkDeviceMemory m_IndexBufferMemory;
	//VkBuffer m_UniformBuffer;
	//VkDeviceMemory m_UniformBufferMemory;
	//VkDescriptorSet m_DescriptorSet;

	UniformBufferObject MVP;

	//Used for 3D models
	//VkBuffer vertexBuffer;
	//VkDeviceMemory vertexBufferMemory;
	//VkBuffer indexBuffer;
	//VkDeviceMemory indexBufferMemory;
	//--
	//Used for matrices buffering (every frame)
	//VkBuffer uniformBuffer;
	//VkDeviceMemory uniformBufferMemory;
	VkDescriptorPool descriptorPool;
	//VkDescriptorSet descriptorSet;
	//--

	//Used for texture
	//VkImage m_TextureImage;
	//VkDeviceMemory m_TextureImageMemory;
	//VkImageView m_TextureImageView;
	//VkSampler m_TextureSampler;
	//--

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkCommandPool commandPool;
	eastl::vector<VkCommandBuffer> commandBuffers;
	//--

	//Semaphores
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	//--

	//Functions
	void CreateInstance();
	void SetupDebugCallback();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline(char* vertPath, char* fragPath);
	void CreateCommandPool();
	void CreateTextureImage();
	
	
	void CreateCommandBuffers();
	void CreateSemaphores();
	void CreateTextureImageView();
	friend class VulkanMesh;
	template<typename T>
	void CreateBuffer(eastl::vector<T> bufferData, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkBufferUsageFlagBits bufferUsageFlagBits);
	void CreateUniformBuffer(VkBuffer* uniformBuffer, VkDeviceMemory* uniformBufferMemory);
	void CreateDescriptorPool();
	
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void SetupIMGUI();
	VkFormat FindSupportedFormat(const eastl::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkCommandBuffer BeginFrame(int imageIndex);
	void EndFrame(int imageIndex);

	//void BindShizzle(VkCommandBuffer cmdBuffer);


	//Functions for new models
	void RecordDrawCommand(VkCommandBuffer cmdBuffer, VkBuffer* myVertexBuffer, VkBuffer* myIndexBuffer);

	//--
	void CreatePrivateCmdBuffers();

	eastl::vector<VkCommandBuffer> myCommandBuffers;

public:
	static VkImageView CreateImageView(VkImage image, VkFormat format, VkDevice* device, VkImageAspectFlags aspectFlags);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void CreateTextureImage(int texWidth, int texHeight, unsigned char* pixels, VkImage* textureImage, VkDeviceMemory* textureImageMemory);
	void CreateTextureImageView(VkImageView* textureImageView, VkImage* textureImage);
	void CreateTextureSampler(VkSampler* sampler, bool isFont);
	void CreateDescriptorSet(VkDescriptorSet* descriptorSet, VkBuffer* uniformBuffer, VkImageView* imageView, VkSampler* textureSampler);

	private:
	uint32_t imageIndex;
	void CleanupSwapChain();

	void UpdateUniformBuffer(VkDeviceMemory uniformBufferMemory);
	void DrawFrame();

	VkShaderModule CreateShaderModule(const eastl::vector<char>&);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	bool IsDeviceSuitable(VkPhysicalDevice);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice);
	bool CheckValidationLayerSupport();

	eastl::vector<const char*> GetRequiredExtensions();

	
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
		uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
	{
		std::cerr << "validation layer: " << msg << std::endl;
		return VK_FALSE;
	}

	float timepassed = 0.f;

private:
	const eastl::vector<Vertex> vertices = {
		{ { -1.0f, -1.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 1.0f, -1.0f,  1.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 1.0f,  1.0f,  1.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -1.0f,  1.0f,  1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

		{ { -1.0f, -1.0f, -1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 1.0f, -1.0f, -1.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 1.0f,  1.0f, -1.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } }
	};

	const eastl::vector<uint32_t> indices = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3
	};

	const eastl::vector<Vertex> verticesTest = {
		{ { -1.2f, -1.2f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 1.2f, -1.2f,  1.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 1.2f,  1.2f,  1.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -1.2f,  1.2f,  1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

		{ { -1.0f, -1.0f, -1.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 1.0f, -1.0f, -1.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 1.0f,  1.0f, -1.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } }
	};
};



template <typename T>
void VulkanRenderer::CreateBuffer(eastl::vector<T> bufferData, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkBufferUsageFlagBits bufferUsageFlagBits)
{
	VkDeviceSize bufferSize = sizeof(bufferData[0]) * bufferData.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, bufferData.data(), size_t(bufferSize));
	vkUnmapMemory(device, stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsageFlagBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

	CopyBuffer(stagingBuffer, buffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}
} // namespace Engine