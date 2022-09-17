#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Window/OpenGLWindow.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Utility/Logging.hpp"

#include <string>

namespace Engine
{
	OpenGLWindow::OpenGLWindow() noexcept {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

		CreateInternalWindow();

		int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
		
		const int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
		const int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
		const int revision = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
		debug_info("OpenGLWindow", "Constructor",
		           std::string(std::string("OpenGL Version ") + std::to_string(major) + "." + std::to_string(
			           minor) + "." + std::to_string(revision)));

		glfwMakeContextCurrent(window);
		//glfwSwapInterval(1);

		glewExperimental = true; // Required because we use the Core profile (line 15)
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK) {
			debug_error("OpenGLWindow", "Constructor", "Failed to initialize OpenGL context");
			return;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);
		// Cull triangles which normal is not towards the camera
		glEnable(GL_CULL_FACE);
	}
} //namespace Engine
#endif