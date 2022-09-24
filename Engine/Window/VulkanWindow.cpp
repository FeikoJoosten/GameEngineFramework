#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include "Engine/Window/VulkanWindow.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"

namespace Engine {
	VulkanWindow::VulkanWindow() noexcept {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		CreateInternalWindow();
	}
}
#endif
