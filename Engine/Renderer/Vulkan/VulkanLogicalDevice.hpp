#pragma once
#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>

#include "Engine/Renderer/Vulkan/VulkanPhysicalDevice.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine {

	class VulkanLogicalDevice
	{
	public:
		VulkanLogicalDevice(VulkanPhysicalDevice* vulkanPhysicalDevice, bool debug, eastl::vector<const char*> extensions, VulkanDeviceFeatures_t requestedDeviceFeatures);
		~VulkanLogicalDevice();

		VkDevice GetDevice() const;

		VulkanPhysicalDevice* GetPhysicalDevice() const;

		VkQueue GetGraphicsQueue() const;
		VkQueue GetComputeQueue() const;
		VkQueue GetPresentQueue() const;

	protected:
		VkDevice device;

		VulkanPhysicalDevice* vulkanPhysicalDevice;

		VkQueue graphicsQueue;
		VkQueue computeQueue;
		VkQueue presentQueue;
	};

}

#endif // USING_VULKAN