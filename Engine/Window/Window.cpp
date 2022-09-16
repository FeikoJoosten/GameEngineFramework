#include "Engine/Window/Window.hpp"


#include "Engine/Engine.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Utility/Logging.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Utility/Utility.hpp"

#ifdef USING_VULKAN
#include "Engine/Renderer/VulkanRenderer.hpp"
#endif

#include <fstream>
#include <cereal/archives/json.hpp>

void error_callback(int error, const char* description) {
	std::string errorcallback = "Error: ";
	errorcallback.append(description);

	debug_error("Window", "Error_callback", errorcallback);
}

struct DestroyglfwWin {
	void operator()(GLFWwindow* ptr) const {
		glfwDestroyWindow(ptr);
	}
};

namespace Engine {

	Window::Window() noexcept {
	settingsPath = Engine::ENGINE_SETTINGS_PATH + std::string{ NAMEOF_TYPE(Window) } + "/" + std::string{ NAMEOF_TYPE(WindowInitializationData) };
		WindowInitializationData initializationData{};
		if (Utility::FileExists(settingsPath)) {
			std::ifstream inputStream(settingsPath);
			if (inputStream.good()) {
				cereal::JSONInputArchive archive(inputStream);
				archive(initializationData);
			}
		}
		width = initializationData.windowWidth;
		height = initializationData.windowHeight;
		title = initializationData.windowTitle;
	}

	Window::~Window() noexcept {
		debug_info("Window", "~Window", std::string("Destroying Window"))
		glfwDestroyWindow(window.get());
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

	//TODO allow this callback to be virtual
	void Window::OnWindowResized(GLFWwindow* glfwWindow, int width, int height) {
		if (width == 0 || height == 0) return;

		const std::shared_ptr<Window> window = Window::Get();
		window->SetWidth(width);
		window->SetHeight(height);

		int display_w, display_h;
		glfwGetFramebufferSize(glfwWindow, &display_w, &display_h);
		window->displayWidth = display_w;
		window->displayHeight = display_h;

		// Update display size for gainput
		InputManager::Get()->GetInputManager().SetDisplaySize(width, height);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(float(width), float(height));
		io.DisplayFramebufferScale = ImVec2(width > 0 ? (float(display_w) / width) : 0, height > 0 ? (float(display_h) / height) : 0);

#ifdef USING_VULKAN
		Renderer::Get<VulkanRenderer>()->Resized();
#endif
	}

	bool Window::ShouldClose() const noexcept {
		if (window != nullptr)
			return glfwWindowShouldClose(window.get());
		return true;
	}

	void Window::SetShouldClose(bool value) const noexcept {
		if (window != nullptr)
			glfwSetWindowShouldClose(window.get(), int(value));
	}

	HWND Window::GetWindowHandle() const noexcept {
		return glfwGetWin32Window(window.get());
	}

	std::weak_ptr<GLFWwindow> Window::GetGLFWWindow() const noexcept {
		return window;
	}

	std::string Window::GetTitle() const {
		return title;
	}

	int Window::GetWidth() const noexcept {
		return width;
	}

	int Window::GetDisplayWidth() const noexcept {
		return displayWidth;
	}

	int Window::GetHeight() const noexcept {
		return height;
	}

	int Window::GetDisplayHeight() const noexcept {
		return displayHeight;
	}

	void Window::CreateInternalWindow() {
		glfwSetErrorCallback(error_callback);
		if (!glfwInit()) {
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = std::unique_ptr<GLFWwindow, DestroyglfwWin>(glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr));

		if (!window) {
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwGetFramebufferSize(window.get(), &displayWidth, &displayHeight);

		glfwSetWindowSizeCallback(window.get(), OnWindowResized);


		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
		io.DisplayFramebufferScale = ImVec2(width > 0 ? (static_cast<float>(displayWidth) / io.DisplaySize.x) : 0, height > 0 ? (static_cast<float>(displayHeight) / io.DisplaySize.y) : 0);
	}

	void Window::Update() const noexcept {}

	void Window::SwapBuffers() const noexcept {
		glfwSwapBuffers(window.get());
	}

	void Window::SetWidth(int newWidth) {
		width = newWidth;
	}

	void Window::SetHeight(int newHeight) {
		height = newHeight;
	}

} //namespace Engine