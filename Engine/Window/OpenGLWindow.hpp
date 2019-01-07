#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#define GLEW_STATIC
#include <ThirdParty/glew-2.1.0/include/GL/glew.h>
#include "Engine/Window/Window.hpp"

namespace Engine
{
	/// <summary>
	/// This object stores any information regarding the created GLFW OpenGLWindow.
	/// </summary>
	class ENGINE_API OpenGLWindow : public Window
	{
		friend class Engine;

		OpenGLWindow() = delete;
		OpenGLWindow(int width, int height, const char* title) noexcept;
		OpenGLWindow(OpenGLWindow const &other) = default;
		OpenGLWindow(OpenGLWindow &&other) noexcept = default;
	public :
		~OpenGLWindow() noexcept override;
	private:
		eastl::shared_ptr<Window> windowReference;
	};
} //namespace Engine
#endif