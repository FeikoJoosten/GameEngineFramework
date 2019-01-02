// Written by Koen Buitenhuis

#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Engine/Window/Window.hpp"
#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		eastl::vector<VkSurfaceFormatKHR> formats;
		eastl::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices {
		int graphicsFamily = -1;
		int presentFamily = -1;

		bool isComplete() {
			return graphicsFamily >= 0 && presentFamily >= 0;
		}
	};

	class ENGINE_API VulkanWindow : public Window
	{
	public:

		VulkanWindow() = delete;
		VulkanWindow(int width, int height, const char* title) noexcept;
		VulkanWindow(VulkanWindow const &other) = default;
		VulkanWindow(VulkanWindow &&other) noexcept = default;
		~VulkanWindow() noexcept;

		void InitSurface(VkInstance* instance);
		void CreateRenderPass(VkDevice* device, VkPhysicalDevice* physicalDevice);

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) noexcept;
		void CreateSwapChain(VkDevice* device, VkPhysicalDevice* physicalDevice);
		void CreateSwapChainFrameBuffers(VkDevice* device);
		void CreateImageViews(VkDevice* device) noexcept;
		void CreateDepthRecources(VkDevice* device, VkPhysicalDevice* physicalDevice);

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice* device) noexcept;
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const eastl::vector<VkSurfaceFormatKHR>& availableFormats) noexcept;
		VkPresentModeKHR ChooseSwapPresentMode(const eastl::vector<VkPresentModeKHR> availablePresentModes) noexcept;
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice* physicalDevice) noexcept;

		void DestroyDepthRecourses(VkDevice* device);
		void DestroySurface(VkInstance* instance) noexcept;
		void DestroyFrameBuffers(VkDevice* device) noexcept;
		void DestroyImageViews(VkDevice* device) noexcept;

		VkSurfaceKHR GetSurface();
		VkRenderPass GetRenderPass();
		VkSwapchainKHR GetSwapChain();
		VkExtent2D GetSwapChainExtent();
		eastl::vector<VkFramebuffer> GetSwapChainFramebuffers();

	private:
		Window* windowReference;
		VkSurfaceKHR surface;
		VkRenderPass renderPass;
		VkSwapchainKHR swapChain;
		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;

		//Depth buffer
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		eastl::vector<VkImage> swapChainImages;
		eastl::vector<VkImageView> swapChainImageViews;
		eastl::vector<VkFramebuffer> swapChainFrameBuffers;

		VkFormat findDepthFormat(VkPhysicalDevice* physicalDevice);
		VkFormat findSupportedFormat(VkPhysicalDevice* physicalDevice, const eastl::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	};
} //namespace Engine