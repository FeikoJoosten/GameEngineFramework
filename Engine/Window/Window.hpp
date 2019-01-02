#pragma once

#include <Engine/api.hpp>
#include <Engine/Utility/Defines.hpp>

#if _WIN32 || _WIN64
#define NOMINMAX
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_INCLUDE_NONE
#if _WIN64
#include <ThirdParty/glfw-3.2.1/x64/include/GLFW/glfw3.h>
#include <ThirdParty/glfw-3.2.1/x64/include/GLFW/glfw3native.h>
#else
#include <ThirdParty/glfw-3.2.1/x86/include/GLFW/glfw3.h>
#include <ThirdParty/glfw-3.2.1/x86/include/GLFW/glfw3native.h>
#endif
#endif

namespace Engine 
{

class ENGINE_API Window
{
public:

	static void OnWindowResized(GLFWwindow*, int, int);

	Window() = default;
	Window(int width, int height, const char* title) noexcept;
	Window(Window const &other) = default;
	Window(Window &&other) noexcept = default;
	virtual ~Window() noexcept;

	Window &operator=(Window const &other) = default;
	Window &operator=(Window &&other) noexcept = default;

	bool ShouldClose() noexcept;
	void SetShouldClose(bool value) noexcept;
	void Update() noexcept;

	HWND GetWindowHandle() noexcept;
	GLFWwindow* GetGLFWWindow() noexcept;
	int GetWidth() noexcept;
	int GetHeight() noexcept;
	void SwapBuffers() noexcept;

protected:
	int width;
	int height;
	GLFWwindow* window;
private:
	void SetWidth(int newWidth);
	void SetHeight(int newHeight);

};
} //namespace Engine
