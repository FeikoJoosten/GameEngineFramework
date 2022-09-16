#include "Engine/Renderer/Vulkan/VulkanLogicalDevice.hpp"
#include <string>
#ifdef USING_VULKAN

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace Engine {
	

	VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice * vulkanPhysicalDevice, bool debug, std::vector<const char*> extensions, VulkanDeviceFeatures_t requestedDeviceFeatures)
	{
		std::vector<VkDeviceQueueCreateInfo> queues;
		std::set<int> uniqueQueueFamilies = { vulkanPhysicalDevice->GetQueueFamilies().graphics,
			vulkanPhysicalDevice->GetQueueFamilies().compute, 
			vulkanPhysicalDevice->GetQueueFamilies().present };
		float queuePriority = 1.f;

		for (int queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo QueueCreateInfo = {};
			QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueCreateInfo.queueFamilyIndex = queueFamily;
			QueueCreateInfo.queueCount = 1;

			QueueCreateInfo.pQueuePriorities = &queuePriority;

			queues.push_back(QueueCreateInfo);

		}

		VkBool32 physicalDeviceFeatures[sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32)];
		float* requestedFeatures = reinterpret_cast<float*>(&requestedDeviceFeatures);

		for (int i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i) {
			if (requestedFeatures[i] > 0.f)
				physicalDeviceFeatures[i] = VK_TRUE;
			else
				physicalDeviceFeatures[i] = VK_FALSE;
		}

		VkPhysicalDeviceFeatures features = {};
		memcpy(&features, &physicalDeviceFeatures, sizeof(VkPhysicalDeviceFeatures));

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queues.size());
		createInfo.pQueueCreateInfos = queues.data();

		createInfo.pEnabledFeatures = &features;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		std::vector<const char*> layers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		if (debug) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
			createInfo.ppEnabledLayerNames = layers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}
		
		VkResult res = vkCreateDevice(vulkanPhysicalDevice->GetPhysicalDevice(), &createInfo, nullptr, &device);

		if (res != VK_SUCCESS) {
			std::string s = std::string("[ERROR] [CODE:") + std::to_string(res).c_str() + "] Logical device creation failed";
			std::cout << s.c_str() << std::endl;
		}

		vkGetDeviceQueue(device, vulkanPhysicalDevice->GetQueueFamilies().graphics, 0, &graphicsQueue);
		vkGetDeviceQueue(device, vulkanPhysicalDevice->GetQueueFamilies().compute, 0, &computeQueue);
		vkGetDeviceQueue(device, vulkanPhysicalDevice->GetQueueFamilies().present, 0, &presentQueue);

		this->vulkanPhysicalDevice = vulkanPhysicalDevice;

	}

	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		vkDestroyDevice(device, nullptr);
	}

	VkDevice VulkanLogicalDevice::GetDevice() const
	{
		return device;
	}

	VulkanPhysicalDevice * VulkanLogicalDevice::GetPhysicalDevice() const
	{
		return vulkanPhysicalDevice;
	}

	VkQueue VulkanLogicalDevice::GetGraphicsQueue() const
	{
		return graphicsQueue;
	}

	VkQueue VulkanLogicalDevice::GetComputeQueue() const
	{
		return computeQueue;
	}

	VkQueue VulkanLogicalDevice::GetPresentQueue() const
	{
		return presentQueue;
	}
}

#endif // USING_VULKAN