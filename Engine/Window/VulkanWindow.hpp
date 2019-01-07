#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#define GLFW_INCLUDE_VULKAN
#include "Engine/Window/Window.hpp"
#include <ThirdParty/Vulkan/Include/vulkan/vulkan.h>

namespace Engine
{
	class ENGINE_API VulkanWindow : public Window
	{
		friend class Engine;

	public:
		VkSurfaceKHR CreateSurface(VkInstance instance);

	private:

		VulkanWindow() = delete;
		VulkanWindow(int width, int height, const char* title) noexcept;
		VulkanWindow(VulkanWindow const &other) = default;
		VulkanWindow(VulkanWindow &&other) noexcept = default;
	public:
		~VulkanWindow() noexcept = default;
	private:

		eastl::shared_ptr<Window> windowReference;
		VkSurfaceKHR surface;
	};
} //namespace Engine
#endif