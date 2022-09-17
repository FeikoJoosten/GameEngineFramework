#pragma once

#include "Engine/Utility/Defines.hpp"
#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Utility/Event.hpp"

#if _WIN32 || _WIN64
#define NOMINMAX
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

#include <memory>
#include <string>

namespace Engine {
	/// <summary>
	/// This object stores any information regarding the created GLFW window.
	/// </summary>
	class ENGINE_API Window {
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
			std::string windowTitle {};

			template <class Archive>
			void Serialize(Archive& archive);
		};

	protected:
		Window() noexcept;
	public:

		virtual ~Window() noexcept;
		Window(const Window& other) = delete;
		Window(Window&& other) noexcept = delete;

		Window& operator=(const Window& other) = delete;
		Window& operator=(Window&& other) noexcept = delete;

		static std::shared_ptr<Window> Get();

		template<typename WindowType>
		static std::shared_ptr<WindowType> Get();

		/// <summary>
		/// This method returns if this window should close.
		/// </summary>
		/// <returns>Returns true if this window should close, will return false otherwise.</returns>
		[[nodiscard]] bool ShouldClose() const noexcept;

		/// <summary>
		/// This method allows you to close the window.
		/// </summary>
		/// <param name="value">Use true to close this window, false to do nothing.</param>
		/// <returns></returns>
		void SetShouldClose(bool value) noexcept;

		/// <summary>
		/// The hardware handle to this window.
		/// </summary>
		/// <returns>Returns the hardware handle to this window.</returns>
		[[nodiscard]] HWND GetWindowHandle() const noexcept;

		/// <summary>
		/// Allows you to get the glfw pointer.
		/// </summary>
		/// <returns>Returns the glfw pointer as a shared pointer.</returns>
		[[nodiscard]] GLFWwindow* GetGlfwWindow() const noexcept;

		/// <summary>
		/// This method allows you to get the title of this window.
		/// </summary>
		/// <returns>Returns the title of this window</returns>
		[[nodiscard]] std::string GetTitle() const;

		/// <summary>
		/// This method allows you to get the width of this window.
		/// </summary>
		/// <returns>Returns the width of this window as an int.</returns>
		[[nodiscard]] int GetWidth() const noexcept;

		/// <summary>
		/// This method allows you to set the width of this window.
		/// </summary>
		/// <param name="newWidth">The new width of this window.</param>
		void SetWidth(int newWidth);

		/// <summary>
		/// This method allows you to get the display width of this window.
		/// </summary>
		/// <returns>Returns the display width of this window as an int.</returns>
		[[nodiscard]] int GetDisplayWidth() const noexcept;

		/// <summary>
		/// This method allows you to get the height of this window.
		/// </summary>
		/// <returns>Returns the height of this window as an int.</returns>
		[[nodiscard]] int GetHeight() const noexcept;

		/// <summary>
		/// This method allows you to set the height of this window.
		/// </summary>
		/// <param name="newHeight">The new height of this window.</param>
		void SetHeight(int newHeight);

		/// <summary>
		/// This method allows you to get the display height of this window.
		/// </summary>
		/// <returns>Returns the display height of this window as an int.</returns>
		[[nodiscard]] int GetDisplayHeight() const noexcept;

		Sharp::Event<GLFWwindow*, int, int> OnWindowResizedEvent;
		Sharp::Event<std::shared_ptr<Window>> OnWindowShutdownRequestedEvent;

	protected:
		int width {};
		int displayWidth {};
		int height {};
		int displayHeight {};
		GLFWwindow* window = nullptr;
		std::string title {};

		/// <summary>
		/// This method is called whenever the GLFW window is resized.
		/// </summary>
		/// <param name="glfwWindow">This is a reference to the window pointer.</param>
		/// <param name="newWidth">The new width.</param>
		/// <param name="newHeight">The new height.</param>
		virtual void OnWindowResized(GLFWwindow* glfwWindow, int newWidth, int newHeight);

		void CreateInternalWindow();

	private:
		std::string settingsPath {};

		static void WindowResizeCallback(GLFWwindow* glfwWindow, const int width, const int height);
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
