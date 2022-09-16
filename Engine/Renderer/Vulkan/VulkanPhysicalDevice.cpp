#include "Engine/Renderer/Vulkan/VulkanPhysicalDevice.hpp"
#ifdef USING_VULKAN

#include "Engine/Renderer/Vulkan/VulkanInstance.hpp"
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>

namespace Engine {

	static VulkanDeviceQueueFamilies_t GetDeviceQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
		VulkanDeviceQueueFamilies_t families = {};

		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
		std::vector<VkQueueFamilyProperties> properties(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, properties.data());

		for (int i = 0; i < static_cast<int>(properties.size()); i++) {
			if (properties[i].queueCount > 0 && properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				families.graphics = i;
			}
			if (properties[i].queueCount > 0 && properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				families.compute = i;
			}
			if (properties[i].queueCount > 0 && properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
				families.transfere = i;
			}
			if (properties[i].queueCount > 0 && properties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
				families.sparseBinding = i;
			}

			VkBool32 presentSupport = false;

			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport && properties[i].queueCount > 0)
				families.present = i;

			if (families.graphics >= 0 && families.compute >= 0 && families.transfere >= 0 && families.sparseBinding >= 0 && families.present >= 0)
				break;
		}

		return families;

	}

	static VulkanSwapChainSupportDetails_t GetDeviceSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {
		VulkanSwapChainSupportDetails_t details = {};

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VulkanPhysicalDevice * VulkanPhysicalDevice::GetBestPhysicalDevice(VulkanInstance * instance, VkSurfaceKHR surface, std::vector<const char*> requiredExtensions, VulkanDeviceFeatures_t requiredFeatures, VulkanDeviceFeatures_t optionalFeatures, VulkanDeviceQueueRequestedFamilies_t requestedQueueFamilies, bool GPU)
	{
		//return nullptr;
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, nullptr);
		if (deviceCount < 1)
			return nullptr;
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, devices.data());
		std::multimap<float, VkPhysicalDevice> selectedDevices;
		for (int i = 0; i < static_cast<int>(devices.size()); i++) {
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
			vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);


			if (GPU && !(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)) {
				continue;
			}

			uint32_t extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensionCount, extensions.data());

			bool extensionNotFound = false;
			for (int i = 0; i < static_cast<int>(requiredExtensions.size()); i++) {
				bool extensionFound = false;
				for (int j = 0; j < static_cast<int>(extensions.size()); j++) {
					if (strcmp(extensions[j].extensionName, requiredExtensions[i]) == 0) {
						extensionFound = true;
						break;
					}
				}
				if (extensionFound == false) {
					extensionNotFound = true;
					break;
				}
			}

			if (extensionNotFound)
				continue;

			VulkanDeviceQueueFamilies_t families = GetDeviceQueueFamilies(devices[i], surface);

			if (requestedQueueFamilies.graphics == true && families.graphics < 0)
				continue;
			if (requestedQueueFamilies.compute == true && families.compute < 0)
				continue;
			if (requestedQueueFamilies.transfere == true && families.transfere < 0)
				continue;
			if (requestedQueueFamilies.sparseBinding == true && families.sparseBinding < 0)
				continue;
			if (requestedQueueFamilies.present == true && families.present < 0)
				continue;

			VulkanSwapChainSupportDetails_t details = GetDeviceSwapChainSupportDetails(devices[i], surface);

			if (details.formats.size() == 0 || details.presentModes.size() == 0)
				continue;

			float* requiredFeaturesValues = (float*)&requiredFeatures;
			VkBool32* requiredFeaturesFoundValues = (VkBool32*)&deviceFeatures;
			bool success = true;
			for (int j = 0; j < sizeof(VulkanDeviceFeatures_t) / sizeof(float); j++) {
				if (requiredFeaturesValues[j] > 0.f&&requiredFeaturesFoundValues[j] != VK_TRUE) {
					success = false;
					break;
				}
				if (requiredFeaturesValues[j] < 0.f&&requiredFeaturesFoundValues[j] == VK_TRUE) {
					success = false;
					break;
				}
			}
			if (success) {
				float* optionalFeaturesValues = (float*)&optionalFeatures;
				float score = 0.01f;
				for (int j = 0; j < sizeof(VulkanDeviceFeatures_t) / sizeof(float); j++) {
					if (optionalFeaturesValues[j] != 0.f) {
						if (optionalFeaturesValues[j] > 0.f&&requiredFeaturesFoundValues[j] == VK_TRUE)
							score += optionalFeaturesValues[j];
						if (optionalFeaturesValues[j] < 0.f&&requiredFeaturesFoundValues[j] != VK_TRUE)
							score += -1.f*optionalFeaturesValues[j];
					}
				}
				selectedDevices.insert(std::make_pair(score, devices[i]));
			}
		}
		if (selectedDevices.rbegin()->first > 0.f) {
			return new VulkanPhysicalDevice(selectedDevices.rbegin()->second, surface);
		}
		return nullptr;
	}
	std::vector<VulkanPhysicalDevice*> VulkanPhysicalDevice::GetAllPhysicalDevices(VulkanInstance* instance, VkSurfaceKHR surface)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, nullptr);
		std::vector<VulkanPhysicalDevice*> Vulkandevices(deviceCount);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, devices.data());
		for (int i = 0; i < static_cast<int>(devices.size()); i++) {
			Vulkandevices[i] = new VulkanPhysicalDevice(devices[i], surface);
		}
		return Vulkandevices;
	}

	VulkanDeviceQueueFamilies_t VulkanPhysicalDevice::GetQueueFamilies()
	{
		//if (!queueFamiliesFound) {
		QueueFamilies = GetDeviceQueueFamilies(device, surface);
		queueFamiliesFound = true;
		//}
		return QueueFamilies;
	}

	VulkanSwapChainSupportDetails_t VulkanPhysicalDevice::GetSwapChainSupportDetails()
	{
		if (!swapChainDetailsFound) {
			details = GetDeviceSwapChainSupportDetails(device, surface);
			swapChainDetailsFound = true;
		}
		return details;
	}
	VulkanSwapChainSupportDetails_t VulkanPhysicalDevice::ReloadSwapChainSupportDetails()
	{
		details = GetDeviceSwapChainSupportDetails(device, surface);
		return details;
	}

	VkSurfaceFormatKHR VulkanPhysicalDevice::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{

		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];

	}

	VkPresentModeKHR VulkanPhysicalDevice::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				//bestMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}

		}

		return bestMode;
	}

	VkExtent2D VulkanPhysicalDevice::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, int width, int height)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			VkExtent2D actualExtend = { static_cast<uint32_t>(width),  static_cast<uint32_t>(height) };

			uint32_t minWidth = actualExtend.width < capabilities.maxImageExtent.width ? actualExtend.width : capabilities.maxImageExtent.width;
			actualExtend.width = capabilities.minImageExtent.width < minWidth ? minWidth : capabilities.minImageExtent.width;

			uint32_t minHeight = actualExtend.height < capabilities.maxImageExtent.height ? actualExtend.height : capabilities.maxImageExtent.height;
			actualExtend.height = capabilities.minImageExtent.height < minHeight ? minHeight : capabilities.minImageExtent.height;

			return actualExtend;
		}
	}

	VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface): queueFamiliesFound(false),
	                                                                                           swapChainDetailsFound(false)
	{
		this->device = device;
		this->surface = surface;
	}


	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
	}
}

#endif // USING_VULKAN