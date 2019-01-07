#pragma once

#include "Engine/Utility/Defines.hpp"
#include "Engine/api.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/string.h>
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>

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
	/// <summary>
	/// This object stores any information regarding the created GLFW window.
	/// </summary>
	class ENGINE_API Window
	{
	public:

		/// <summary>
		/// This method is called whenever the GLFW window is resized.
		/// </summary>
		/// <param name="window">This is a reference to the window pointer.</param>
		/// <param name="width">The new width.</param>
		/// <param name="height">The new height.</param>
		static void OnWindowResized(GLFWwindow* window, int width, int height);

		/// <summary>
		/// This method returns if this window should close.
		/// </summary>
		/// <returns>Returns true if this window should close, will return false otherwise.</returns>
		bool ShouldClose() const noexcept;

		/// <summary>
		/// This method allows you to close the window.
		/// </summary>
		/// <param name="value">Use true to close this window, false to do nothing.</param>
		/// <returns></returns>
		void SetShouldClose(bool value) const noexcept;

		/// <summary>
		/// The hardware handle to this window.
		/// </summary>
		/// <returns>Returns the hardware handle to this window as a HWND.</returns>
		HWND GetWindowHandle() const noexcept;

		/// <summary>
		/// Allows you to get the glfw pointer.
		/// </summary>
		/// <returns>Returns the glfw pointer as a weak pointer.</returns>
		eastl::weak_ptr<GLFWwindow> GetGLFWWindow() const noexcept;

		/// <summary>
		/// This method allows you to get the title of this window.
		/// </summary>
		/// <returns>Returns the title of this window as a std::string</returns>
		eastl::string GetTitle() const;

		/// <summary>
		/// This method allows you to get the width of this window.
		/// </summary>
		/// <returns>Returns the width of this window as an int.</returns>
		int GetWidth() const noexcept;

		/// <summary>
		/// This method allows you to get the display width of this window.
		/// </summary>
		/// <returns>Returns the display width of this window as an int.</returns>
		int GetDisplayWidth() const noexcept;

		/// <summary>
		/// This method allows you to get the height of this window.
		/// </summary>
		/// <returns>Returns the height of this window as an int.</returns>
		int GetHeight() const noexcept;

		/// <summary>
		/// This method allows you to get the display height of this window.
		/// </summary>
		/// <returns>Returns the display height of this window as an int.</returns>
		int GetDisplayHeight() const noexcept;

	protected:
		int width, displayWidth;
		int height, displayHeight;
		eastl::shared_ptr<GLFWwindow> window;
		eastl::string title;

	private:
		friend class Engine;
#ifdef USING_OPENGL
		friend class OpenGLWindow;
#endif
#ifdef USING_VULKAN
		friend class VulkanWindow;
#endif

		Window() = default;
		Window(int width, int height, const char* title) noexcept;
		Window(Window const &other) = delete;
		Window(Window &&other) noexcept = delete;
	public:
		virtual ~Window() noexcept;
	private:

		Window &operator=(Window const &other) = delete;
		Window &operator=(Window &&other) noexcept = delete;

		/// <summary>
		/// General update method for this window. NOTE: Can only be called by the engine.
		/// </summary>
		/// <returns></returns>
		void Update() const noexcept;

		friend class Renderer;
#ifdef USING_OPENGL
		friend class OpenGLRenderer;
#endif
#ifdef USING_VULKAN
		friend class VulkanRenderer;
#endif
		/// <summary>
		/// This method allows you to swap the buffers of this window. NOTE: can only be called by the renderer.
		/// </summary>
		/// <returns></returns>
		void SwapBuffers() const noexcept;

		/// <summary>
		/// This method allows you to set the width of this window.
		/// </summary>
		/// <param name="newWidth">The new width of this window.</param>
		void SetWidth(int newWidth);
		/// <summary>
		/// This method allows you to set the height of this window.
		/// </summary>
		/// <param name="newHeight">The new height of this window.</param>
		void SetHeight(int newHeight);
	};
} //namespace Engine
