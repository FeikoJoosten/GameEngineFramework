// Written by Koen Buitenhuis

#include "Engine/Window/VulkanWindow.hpp"
#include "Engine/engine.hpp"
#include <cstdio>
#include <cstdlib>
#include "Engine/Renderer/VulkanRenderer.hpp"

Engine::VulkanWindow::VulkanWindow(int width, int height, const char* title) noexcept : surface(0), renderPass(0),
                                                                                        swapChain(0),
                                                                                        swapChainImageFormat()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	windowReference = new Window(width, height, title);

	window = windowReference->GetGLFWWindow();
	this->width = windowReference->GetWidth();
	this->height = windowReference->GetHeight();

}

Engine::VulkanWindow::~VulkanWindow() noexcept
{

}

void Engine::VulkanWindow::InitSurface(VkInstance* instance)
{
	if (glfwCreateWindowSurface(*instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}

void Engine::VulkanWindow::DestroySurface(VkInstance* instance) noexcept
{
	vkDestroySurfaceKHR(*instance, surface, nullptr);
}

void Engine::VulkanWindow::CreateDepthRecources(VkDevice* device, VkPhysicalDevice* physicalDevice)
{
	VkFormat depthFormat = findDepthFormat(physicalDevice);
	
	Engine::GetRenderer<VulkanRenderer>()->CreateImage(Engine::GetWindow<VulkanWindow>()->GetSwapChainExtent().width, Engine::GetWindow<VulkanWindow>()->GetSwapChainExtent().height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = Engine::GetRenderer<VulkanRenderer>()->CreateImageView(depthImage, depthFormat, device, VK_IMAGE_ASPECT_DEPTH_BIT);


	Engine::GetRenderer<VulkanRenderer>()->TransitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

void Engine::VulkanWindow::CreateRenderPass(VkDevice* device, VkPhysicalDevice* physicalDevice)
{
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat(physicalDevice);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	eastl::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(*device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

VkExtent2D Engine::VulkanWindow::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) noexcept
{
	if (capabilities.currentExtent.width != eastl::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}

	VkExtent2D actualExtent = { uint32_t(width), uint32_t(height) };

	actualExtent.width = eastl::max(capabilities.minImageExtent.width, eastl::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = eastl::max(capabilities.minImageExtent.height, eastl::min(capabilities.maxImageExtent.height, actualExtent.height));

	return actualExtent;
}

void Engine::VulkanWindow::CreateSwapChain(VkDevice* device, VkPhysicalDevice* physicalDevice)
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
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

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { uint32_t(indices.graphicsFamily), uint32_t(indices.presentFamily) };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(*device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(*device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(*device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void Engine::VulkanWindow::CreateSwapChainFrameBuffers(VkDevice* device)
{
	size_t swapChainImageViewsSize = swapChainImageViews.size();

	swapChainFrameBuffers.resize(swapChainImageViewsSize);

	for (size_t i = 0; i < swapChainImageViewsSize; ++i)
	{
		eastl::array<VkImageView, 2> attachments =
		{
			swapChainImageViews[i],
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(*device, &framebufferInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Engine::VulkanWindow::CreateImageViews(VkDevice* device) noexcept
{
	size_t swapChainImagesSize = swapChainImages.size();

	swapChainImageViews.resize(swapChainImagesSize);
	for (uint32_t i = 0; i < swapChainImagesSize; ++i)
	{
		swapChainImageViews[i] = Engine::Engine::GetRenderer<VulkanRenderer>()->CreateImageView(swapChainImages[i], swapChainImageFormat, device, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

Engine::SwapChainSupportDetails Engine::VulkanWindow::QuerySwapChainSupport(VkPhysicalDevice* device) noexcept
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(*device, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(*device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(*device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(*device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR Engine::VulkanWindow::ChooseSwapSurfaceFormat(const eastl::vector<VkSurfaceFormatKHR>& availableFormats) noexcept
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Engine::VulkanWindow::ChooseSwapPresentMode(const eastl::vector<VkPresentModeKHR> availablePresentModes) noexcept
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}

		if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			return availablePresentMode;
		}
	}

	return bestMode;
}

Engine::QueueFamilyIndices Engine::VulkanWindow::FindQueueFamilies(VkPhysicalDevice* physicalDevice) noexcept
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, nullptr);

	eastl::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, queueFamilies.data());

	QueueFamilyIndices indices;
	for (size_t i = 0, end = queueFamilies.size(); i < end; ++i)
	{
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = int(i);
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, uint32_t(i), surface, &presentSupport);

		if (queueFamilies[i].queueCount > 0 && presentSupport)
		{
			indices.presentFamily = int(i);
		}

		if (indices.isComplete())
		{
			break;
		}
	}

	return indices;
}

void Engine::VulkanWindow::DestroyDepthRecourses(VkDevice* device)
{
	vkDestroyImageView(*device, depthImageView, nullptr);
	vkDestroyImage(*device, depthImage, nullptr);
	vkFreeMemory(*device, depthImageMemory, nullptr);
}

void Engine::VulkanWindow::DestroyFrameBuffers(VkDevice* device) noexcept
{
	for (size_t i = 0, size = swapChainFrameBuffers.size(); i < size; i++)
	{
		vkDestroyFramebuffer(*device, swapChainFrameBuffers[i], nullptr);
	}
}

void Engine::VulkanWindow::DestroyImageViews(VkDevice* device) noexcept
{
	for (size_t i = 0, size = swapChainImageViews.size(); i < size; i++)
	{
		vkDestroyImageView(*device, swapChainImageViews[i], nullptr);
	}
}

VkSurfaceKHR Engine::VulkanWindow::GetSurface()
{
	return surface;
}

VkRenderPass Engine::VulkanWindow::GetRenderPass()
{
	return renderPass;
}

VkSwapchainKHR Engine::VulkanWindow::GetSwapChain()
{
	return swapChain;
}

VkExtent2D Engine::VulkanWindow::GetSwapChainExtent()
{
	return swapChainExtent;
}

eastl::vector<VkFramebuffer> Engine::VulkanWindow::GetSwapChainFramebuffers()
{
	return swapChainFrameBuffers;
}

VkFormat Engine::VulkanWindow::findSupportedFormat(VkPhysicalDevice* physicalDevice, const eastl::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(*physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VkFormat Engine::VulkanWindow::findDepthFormat(VkPhysicalDevice* physicalDevice) {
	return findSupportedFormat(physicalDevice,
	{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}