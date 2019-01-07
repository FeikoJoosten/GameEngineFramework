#include "Engine/Window/Window.hpp"
#include "Engine/engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Utility/Logging.hpp"
#include "Engine/Input/InputManager.hpp"

#ifdef USING_VULKAN
#include "Engine/Renderer/VulkanRenderer.hpp"
#endif

void error_callback(int error, const char* description)
{
	eastl::string errorcallback = "Error: ";
	errorcallback.append(description);

	debug_error("Window", "Error_callback", errorcallback);
}

struct DestroyglfwWin {
	void operator()(GLFWwindow* ptr) const
	{
		glfwDestroyWindow(ptr);
	}
};

namespace Engine
{
	//TODO -- Feiko | allow this callback to be virtual
	void Window::OnWindowResized(GLFWwindow* window, int width, int height)
	{
		if (width == 0 || height == 0) return;

		Engine::GetEngine().lock()->GetWindow().lock()->SetWidth(width);
		Engine::GetEngine().lock()->GetWindow().lock()->SetHeight(height);

		int display_w, display_h;
		glfwGetFramebufferSize(Engine::GetEngine().lock()->GetWindow().lock()->GetGLFWWindow().lock().get(), &display_w, &display_h);
		Engine::GetEngine().lock()->GetWindow().lock()->displayWidth = display_w;
		Engine::GetEngine().lock()->GetWindow().lock()->displayHeight = display_h;

		// Update display size for gainput
		Engine::GetEngine().lock()->GetInputManager().lock()->GetInputManager().SetDisplaySize(width, height);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(float(width), float(height));
		io.DisplayFramebufferScale = ImVec2(width > 0 ? (float(display_w) / width) : 0, height > 0 ? (float(display_h) / height) : 0);

#ifdef USING_VULKAN
		Engine::GetEngine().lock()->GetRenderer<VulkanRenderer>().lock()->Resized();
#endif
	}

	bool Window::ShouldClose() const noexcept
	{
		if (window != nullptr)
			return glfwWindowShouldClose(window.get());
		return true;
	}

	void Window::SetShouldClose(bool value) const noexcept
	{
		if (window != nullptr)
			glfwSetWindowShouldClose(window.get(), int(value));
	}

	HWND Window::GetWindowHandle() const noexcept
	{
		return glfwGetWin32Window(window.get());
	}

	eastl::weak_ptr<GLFWwindow> Window::GetGLFWWindow() const noexcept
	{
		return window;
	}

	eastl::string Window::GetTitle() const
	{
		return title;
	}

	int Window::GetWidth() const noexcept
	{
		return width;
	}

	int Window::GetDisplayWidth() const noexcept
	{
		return displayWidth;
	}

	int Window::GetHeight() const noexcept
	{
		return height;
	}

	int Window::GetDisplayHeight() const noexcept
	{
		return displayHeight;
	}

	Window::Window(int width, int height, const char* title) noexcept : width(width), height(height), title(title)
	{
		glfwSetErrorCallback(error_callback);
		if (!glfwInit())
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = eastl::unique_ptr<GLFWwindow, DestroyglfwWin>(glfwCreateWindow(width, height, title, nullptr, nullptr));

		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwGetFramebufferSize(window.get(), &displayWidth, &displayHeight);

		glfwSetWindowSizeCallback(window.get(), OnWindowResized);


		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(float(width), float(height));
		io.DisplayFramebufferScale = ImVec2(width > 0 ? (float(displayWidth) / width) : 0, height > 0 ? (float(displayHeight) / height) : 0);
	}

	Window::~Window() noexcept
	{
		glfwDestroyWindow(window.get());
		glfwTerminate();
	}

	void Window::Update() const noexcept
	{
	}

	void Window::SwapBuffers() const noexcept
	{
		glfwSwapBuffers(window.get());
	}

	void Window::SetWidth(int newWidth)
	{
		width = newWidth;
	}

	void Window::SetHeight(int newHeight)
	{
		height = newHeight;
	}

} //namespace Engine