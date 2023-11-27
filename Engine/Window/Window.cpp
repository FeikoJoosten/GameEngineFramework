#include "Engine/Window/Window.hpp"

#include "Engine/AssetManagement/EngineAssetManager.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Utility/Logging.hpp"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Engine {

	void ErrorCallback(int error, const char* description) {
		const std::string errorCallback = "Error: ";
		DEBUG_ERROR(errorCallback + description)
	}

	Window::Window() noexcept {
		WindowInitializationData initializationData;
		EngineAssetManager::TryLoadEngineData<Window>(initializationData);

		width = initializationData.windowWidth;
		height = initializationData.windowHeight;
		xPosition = initializationData.windowXPosition;
		yPosition = initializationData.windowYPosition;
		title = initializationData.windowTitle;
	}

	Window::~Window() noexcept {
		glfwDestroyWindow(window);
		glfwTerminate();

		const WindowInitializationData initializationData = {
			width,
			height,
			xPosition,
			yPosition,
			title
		};

		EngineAssetManager::SaveEngineData<Window>(initializationData);
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
			OnWindowShutdownRequestedEvent();

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

	void Window::SetWidth(const int newWidth) const noexcept {
		if(window)
			glfwSetWindowSize(window, newWidth, height);
	}

	int Window::GetDisplayWidth() const noexcept {
		return displayWidth;
	}

	int Window::GetHeight() const noexcept {
		return height;
	}

	void Window::SetHeight(const int newHeight) const noexcept {
		if (window)
			glfwSetWindowSize(window, width, newHeight);
	}

	int Window::GetXPosition() const noexcept {
		return xPosition;
	}

	void Window::SetXPosition(const int newXPosition) const noexcept {
		if(window)
			glfwSetWindowPos(window, newXPosition, yPosition);
	}

	int Window::GetYPosition() const noexcept {
		return yPosition;
	}

	void Window::SetYPosition(const int newYPosition) const noexcept {
		if(window)
			glfwSetWindowPos(window, xPosition, newYPosition);
	}

	int Window::GetDisplayHeight() const noexcept {
		return displayHeight;
	}

	void Window::HandleOnWindowResized(const int newWidth, const int newHeight) {
		if (newWidth == 0 || newHeight == 0 || !window) return;

		width = newWidth;
		height = newHeight;

		int displayW;
		int displayH;
		glfwGetFramebufferSize(window, &displayW, &displayH);
		displayWidth = displayW;
		displayHeight = displayH;

		OnWindowResizedEvent(newWidth, newHeight);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(newWidth), static_cast<float>(newHeight));
		io.DisplayFramebufferScale = ImVec2(newWidth > 0 ? static_cast<float>(displayW) / io.DisplaySize.x : 0, newHeight > 0 ? static_cast<float>(displayH) / io.DisplaySize.y : 0);
	}

	void Window::HandleOnWindowRepositioned(const int newXPosition, const int newYPosition) {
		xPosition = newXPosition;
		yPosition = newYPosition;

		OnWindowRepositionedEvent(newXPosition, newYPosition);
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
		if(xPosition > 0 || yPosition > 0) // Only assign the position if it's not the default, otherwise you cannot move the window with the cursor
			glfwSetWindowPos(window, xPosition, yPosition);

		if (!window) {
			glfwTerminate();
			assert(EXIT_FAILURE);
			return;
		}

		glfwSetWindowSizeCallback(window, WindowResizeCallback);
		glfwSetWindowPosCallback(window, WindowRepositionCallback);
		HandleOnWindowResized(width, height);
	}

	void Window::WindowResizeCallback(GLFWwindow*, const int newWidth, const int newHeight) {
		const std::shared_ptr<Window> window = Get();
		window->HandleOnWindowResized(newWidth, newHeight);
	}

	void Window::WindowRepositionCallback(GLFWwindow*, const int newXPosition, const int newYPosition) {
		const std::shared_ptr<Window> window = Get();
		window->HandleOnWindowRepositioned(newXPosition, newYPosition);
	}

} //namespace Engine