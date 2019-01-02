#include "Engine/Window/OpenGLWindow.hpp"
#include <iostream>

namespace Engine
{
	OpenGLWindow::OpenGLWindow(int width, int height, const char* title) noexcept
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		windowReference = new Window(width, height, title);
		
		window = windowReference->GetGLFWWindow();
		this->width = windowReference->GetWidth();
		this->height = windowReference->GetHeight();

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
		int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
		int revision = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
		std::cout << "OpenGL Version " << major << "." << minor << "." << revision << std::endl;

		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			std::cout << "Failed to initialize OpenGL context" << std::endl;
			return;
		}

		glClearColor(0.0, 0.0, 0.0, 1.0);
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
