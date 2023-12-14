#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include "Engine/Engine/Engine.hpp"
#include "Engine/Platform/Vulkan/Window/VulkanWindow.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanRenderer.hpp"

namespace Engine {
	VulkanWindow::VulkanWindow() noexcept {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		CreateInternalWindow();
	}
}
#endif
