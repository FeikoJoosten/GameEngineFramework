#include "Engine/Utility/Defines.hpp"
#ifdef USING_VULKAN
#include "Engine/Window/VulkanWindow.hpp"
#include "Engine/engine.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"

VkSurfaceKHR Engine::VulkanWindow::CreateSurface(VkInstance instance)
{
	glfwCreateWindowSurface(instance, window.get(), nullptr, &surface);
	return surface;
}

Engine::VulkanWindow::VulkanWindow(int width, int height, const char* title) noexcept : surface(0)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	windowReference = eastl::shared_ptr<Window>(new Window(width, height, title));

	window = windowReference->GetGLFWWindow().lock();
	this->width = windowReference->GetWidth();
	this->height = windowReference->GetHeight();
	this->title = windowReference->GetTitle();
}
#endif
