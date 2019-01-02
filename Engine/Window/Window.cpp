#include "Engine/Window/Window.hpp"
#include "Engine/engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include <cstdio>
#include <cstdlib>

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//TODO -- Feiko | allow this callback to be virtual
void Engine::Window::OnWindowResized(GLFWwindow*, int width, int height)
{
	if (width == 0 || height == 0) return;

	Engine::Engine::GetWindow().SetWidth(width);
	Engine::Engine::GetWindow().SetHeight(height);

	int display_w, display_h;
	glfwGetFramebufferSize(Engine::Engine::GetWindow().GetGLFWWindow(), &display_w, &display_h);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(float(width), float(height));
	io.DisplayFramebufferScale = ImVec2(width > 0 ? (float(display_w) / width) : 0, height > 0 ? (float(display_h) / height) : 0);
}

Engine::Window::Window(int width, int height, const char* title) noexcept : width(width), height(height)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	//glfwSetWindowUserPointer(window, this);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(float(width), float(height));
	io.DisplayFramebufferScale = ImVec2(width > 0 ? (float(display_w) / width) : 0, height > 0 ? (float(display_h) / height) : 0);

	glfwSetWindowSizeCallback(window, OnWindowResized);
}

Engine::Window::~Window() noexcept
{
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

bool Engine::Window::ShouldClose() noexcept
{
	if (window != nullptr)
		return glfwWindowShouldClose(window);
	return true;
}

void Engine::Window::SetShouldClose(bool value) noexcept
{
	if (window != nullptr)
		glfwSetWindowShouldClose(window, int(value));
}

void Engine::Window::Update() noexcept
{
	//Main render loop
}

HWND Engine::Window::GetWindowHandle() noexcept
{
	return glfwGetWin32Window(window);
}

GLFWwindow* Engine::Window::GetGLFWWindow() noexcept
{
	return window;
}

int Engine::Window::GetWidth() noexcept
{
	return width;
}

int Engine::Window::GetHeight() noexcept
{
	return height;
}

void Engine::Window::SwapBuffers() noexcept
{
	glfwSwapBuffers(window);
}

void Engine::Window::SetWidth(int newWidth)
{
	width = newWidth;
}

void Engine::Window::SetHeight(int newHeight)
{
	height = newHeight;
}