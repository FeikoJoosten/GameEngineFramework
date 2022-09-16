#pragma once

#include <memory>
#include <string>
#include "Engine/Utility/Defines.hpp"
#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"

#if _WIN32 || _WIN64
#define NOMINMAX
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

namespace Engine
{
	/// <summary>
	/// This object stores any information regarding the created GLFW window.
	/// </summary>
	class ENGINE_API Window
	{
		struct WindowInitializationData {
			/// <summary>
			/// WindowWidth defines the width of the engine window.
			/// Default is 640.
			/// </summary>
			int windowWidth = 640;
			/// <summary>
			/// WindowHeight defines the height of the engine window.
			/// Default is 480.
			/// </summary>
			int windowHeight = 480;
			/// <summary>
			/// Window Title defines the title of the engine window.
			/// Default is "".
			/// </summary>
			std::string windowTitle{};

			template <class Archive>
			void Serialize(Archive& archive);
		};

		friend class Application;
		friend class Renderer;
#ifdef USING_OPENGL
		friend class OpenGLWindow;
		friend class OpenGLRenderer;
#endif
#ifdef USING_VULKAN
		friend class VulkanWindow;
		friend class VulkanRenderer;
#endif

		Window() noexcept;
	public:

		Window(const Window& other) = delete;
		Window(Window&& other) noexcept = delete;
		virtual ~Window() noexcept;

		Window& operator=(const Window& other) = delete;
		Window& operator=(Window&& other) noexcept = delete;

		static std::shared_ptr<Window> Get();

		template<typename WindowType>
		static std::shared_ptr<WindowType> Get();

		/// <summary>
		/// This method is called whenever the GLFW window is resized.
		/// </summary>
		/// <param name="glfwWindow">This is a reference to the window pointer.</param>
		/// <param name="width">The new width.</param>
		/// <param name="height">The new height.</param>
		static void OnWindowResized(GLFWwindow* glfwWindow, int width, int height);

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
		std::weak_ptr<GLFWwindow> GetGLFWWindow() const noexcept;

		/// <summary>
		/// This method allows you to get the title of this window.
		/// </summary>
		/// <returns>Returns the title of this window as a std::string</returns>
		std::string GetTitle() const;

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
		int width, displayWidth {};
		int height, displayHeight {};
		std::shared_ptr<GLFWwindow> window;
		std::string title {};

		void CreateInternalWindow();
		
		/// <summary>
		/// General update method for this window. NOTE: Can only be called by the engine.
		/// </summary>
		/// <returns></returns>
		void Update() const noexcept;

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

	private:
		std::string settingsPath {};
	};
	
	
	template <class Archive> void Window::WindowInitializationData::Serialize(Archive& archive) {
		archive(CEREAL_NVP(windowWidth),
			CEREAL_NVP(windowHeight),
			CEREAL_NVP(windowTitle));
	}

	template <typename WindowType> std::shared_ptr<WindowType> Window::Get() {
		return Engine::GetWindow<WindowType>();
	}
} //namespace Engine
