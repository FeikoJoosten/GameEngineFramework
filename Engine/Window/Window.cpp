#include "Engine/Window/Window.hpp"

#include "Engine/Engine.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Utility/Logging.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Utility/Utility.hpp"

#include <fstream>
#include <cereal/archives/json.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Engine {

	void ErrorCallback(int error, const char* description) {
		const std::string errorCallback = "Error: ";
		debug_error("Window", "Error_callback", errorCallback + description)
	}

	Window::Window() noexcept {
		settingsPath = Engine::ENGINE_SETTINGS_PATH + std::string { NAMEOF_TYPE(Window) } + "/" + std::string { NAMEOF_TYPE(WindowInitializationData) };
		WindowInitializationData initializationData {};
		if (Utility::FileExists(settingsPath)) {
			if (std::ifstream inputStream(settingsPath); inputStream.good()) {
				cereal::JSONInputArchive archive(inputStream);
				archive(initializationData);
			}
		}
		width = initializationData.windowWidth;
		height = initializationData.windowHeight;
		title = initializationData.windowTitle;
	}

	Window::~Window() noexcept {
		glfwDestroyWindow(window);
		glfwTerminate();

		if (settingsPath.length() <= 0) return;

		WindowInitializationData initializationData = {
			width,
			height,
			title
		};

		std::ofstream outputStream(settingsPath);
		cereal::JSONOutputArchive archive(outputStream);

		archive(CEREAL_NVP(initializationData));
	}

	std::shared_ptr<Window> Window::Get() {
		return Engine::GetWindow();
	}

	bool Window::ShouldClose() const noexcept {
		if (window != nullptr)
			return glfwWindowShouldClose(window);
		return true;
	}

	void Window::SetShouldClose(const bool value) noexcept {
		if (value)
			OnWindowShutdownRequestedEvent(Get());

		if (window != nullptr)
			glfwSetWindowShouldClose(window, value);
	}

	HWND Window::GetWindowHandle() const noexcept {
		return glfwGetWin32Window(window);
	}

	GLFWwindow* Window::GetGlfwWindow() const noexcept {
		return window;
	}

	std::string Window::GetTitle() const {
		return title;
	}

	int Window::GetWidth() const noexcept {
		return width;
	}

	void Window::SetWidth(const int newWidth) {
		if (window)
			OnWindowResized(window, newWidth, height);
	}

	int Window::GetDisplayWidth() const noexcept {
		return displayWidth;
	}

	int Window::GetHeight() const noexcept {
		return height;
	}

	void Window::SetHeight(const int newHeight) {
		if (window)
			OnWindowResized(window, width, newHeight);
	}

	int Window::GetDisplayHeight() const noexcept {
		return displayHeight;
	}

	void Window::OnWindowResized(GLFWwindow* glfwWindow, const int newWidth, const int newHeight) {
		if (newWidth == 0 || newHeight == 0) return;

		width = newWidth;
		height = newHeight;

		int displayW;
		int displayH;
		glfwGetFramebufferSize(glfwWindow, &displayW, &displayH);
		displayWidth = displayW;
		displayHeight = displayH;

		OnWindowResizedEvent(glfwWindow, newWidth, newHeight);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(newWidth), static_cast<float>(newHeight));
		io.DisplayFramebufferScale = ImVec2(newWidth > 0 ? static_cast<float>(displayW) / io.DisplaySize.x : 0, newHeight > 0 ? static_cast<float>(displayH) / io.DisplaySize.y : 0);
	}

	void Window::CreateInternalWindow() {
		if (window) return;

		glfwSetErrorCallback(ErrorCallback);
		if (!glfwInit()) {
			glfwTerminate();
			assert(EXIT_FAILURE);
			return;
		}

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		if (!window) {
			glfwTerminate();
			assert(EXIT_FAILURE);
			return;
		}

		glfwSetWindowSizeCallback(window, WindowResizeCallback);
		OnWindowResized(window, width, height);
	}

	void Window::WindowResizeCallback(GLFWwindow* glfwWindow, const int width, const int height) {
		const std::shared_ptr<Window> window = Get();
		window->OnWindowResized(glfwWindow, width, height);
	}

} //namespace Engine