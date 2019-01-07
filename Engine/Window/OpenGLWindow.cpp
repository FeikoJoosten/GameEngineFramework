#include "Engine/Utility/Defines.hpp"
#include <string>
#ifdef USING_OPENGL
#include "Engine/Window/OpenGLWindow.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Renderer/imgui_impl_glfw_gl3.h"
#include "Engine/Utility/Logging.hpp"
#include <iostream>

namespace Engine
{
	OpenGLWindow::OpenGLWindow(int width, int height, const char* title) noexcept
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		windowReference = eastl::shared_ptr<Window>(new Window(width, height, title));

		window = windowReference->GetGLFWWindow().lock();
		this->width = windowReference->GetWidth();
		this->displayWidth = windowReference->GetDisplayWidth();
		this->displayHeight = windowReference->GetDisplayHeight();
		this->height = windowReference->GetHeight();
		this->title = windowReference->GetTitle();

		int major = glfwGetWindowAttrib(window.get(), GLFW_CONTEXT_VERSION_MAJOR);
		int minor = glfwGetWindowAttrib(window.get(), GLFW_CONTEXT_VERSION_MINOR);
		int revision = glfwGetWindowAttrib(window.get(), GLFW_CONTEXT_REVISION);
		debug_info("OpenGLWindow", "Constructor", eastl::string(eastl::string("OpenGL Version ") + std::to_string(major).c_str() + "." + std::to_string(minor).c_str() + "." + std::to_string(revision).c_str()));

		glfwMakeContextCurrent(window.get());
		//glfwSwapInterval(1);

		glewExperimental = true; // Required because we use the Core profile (line 15)
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
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

	OpenGLWindow::~OpenGLWindow() noexcept
	{
	}
} //namespace Engine
#endif