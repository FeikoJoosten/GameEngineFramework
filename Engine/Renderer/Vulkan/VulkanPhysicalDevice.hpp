#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine {

	class VulkanInstance;

	typedef struct {
		float    robustBufferAccess;
		float    fullDrawIndexUint32;
		float    imageCubeArray;
		float    independentBlend;
		float    geometryShader;
		float    tessellationShader;
		float    sampleRateShading;
		float    dualSrcBlend;
		float    logicOp;
		float    multiDrawIndirect;
		float    drawIndirectFirstInstance;
		float    depthClamp;
		float    depthBiasClamp;
		float    fillModeNonSolid;
		float    depthBounds;
		float    wideLines;
		float    largePoints;
		float    alphaToOne;
		float    multiViewport;
		float    samplerAnisotropy;
		float    textureCompressionETC2;
		float    textureCompressionASTC_LDR;
		float    textureCompressionBC;
		float    occlusionQueryPrecise;
		float    pipelineStatisticsQuery;
		float    vertexPipelineStoresAndAtomics;
		float    fragmentStoresAndAtomics;
		float    shaderTessellationAndGeometryPointSize;
		float    shaderImageGatherExtended;
		float    shaderStorageImageExtendedFormats;
		float    shaderStorageImageMultisample;
		float    shaderStorageImageReadWithoutFormat;
		float    shaderStorageImageWriteWithoutFormat;
		float    shaderUniformBufferArrayDynamicIndexing;
		float    shaderSampledImageArrayDynamicIndexing;
		float    shaderStorageBufferArrayDynamicIndexing;
		float    shaderStorageImageArrayDynamicIndexing;
		float    shaderClipDistance;
		float    shaderCullDistance;
		float    shaderFloat64;
		float    shaderInt64;
		float    shaderInt16;
		float    shaderResourceResidency;
		float    shaderResourceMinLod;
		float    sparseBinding;
		float    sparseResidencyBuffer;
		float    sparseResidencyImage2D;
		float    sparseResidencyImage3D;
		float    sparseResidency2Samples;
		float    sparseResidency4Samples;
		float    sparseResidency8Samples;
		float    sparseResidency16Samples;
		float    sparseResidencyAliased;
		float    variableMultisampleRate;
		float    inheritedQueries;
	}VulkanDeviceFeatures_t;

	typedef struct {
		bool graphics;
		bool compute;
		bool transfere;
		bool sparseBinding;
		bool present;
	}VulkanDeviceQueueRequestedFamilies_t;

	typedef struct {
		int graphics = -1;
		int compute = -1;
		int transfere = -1;
		int sparseBinding = -1;
		int present = -1;
	}VulkanDeviceQueueFamilies_t;

	typedef struct {
		VkSurfaceCapabilitiesKHR capabilities;
		eastl::vector<VkSurfaceFormatKHR> formats;
		eastl::vector<VkPresentModeKHR> presentModes;
	}VulkanSwapChainSupportDetails_t;

	class VulkanPhysicalDevice
	{
	public:
		static VulkanPhysicalDevice* GetBestPhysicalDevice(VulkanInstance* instance, VkSurfaceKHR surface, eastl::vector<const char*> requiredExtensions,
			VulkanDeviceFeatures_t requiredFeatures = {}, VulkanDeviceFeatures_t optionalFeatures = {}, VulkanDeviceQueueRequestedFamilies_t requestedQueueFamilies = {},
			bool GPU = true);
		static eastl::vector<VulkanPhysicalDevice*> GetAllPhysicalDevices(VulkanInstance* instance, VkSurfaceKHR surface);

		VkPhysicalDevice GetPhysicalDevice() { return device; };
		VulkanDeviceQueueFamilies_t GetQueueFamilies();
		VulkanSwapChainSupportDetails_t GetSwapChainSupportDetails();
		VulkanSwapChainSupportDetails_t ReloadSwapChainSupportDetails();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const eastl::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const eastl::vector<VkPresentModeKHR> availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);

		~VulkanPhysicalDevice();

	private:
		VulkanPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);

		VkPhysicalDevice device;
		VulkanDeviceQueueFamilies_t QueueFamilies;
		VulkanSwapChainSupportDetails_t details;
		VkSurfaceKHR surface;
		bool queueFamiliesFound;
		bool swapChainDetailsFound;
	};

}

#endif // USING_VULKAN