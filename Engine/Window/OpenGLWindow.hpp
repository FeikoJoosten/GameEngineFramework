#pragma once

#define GLEW_STATIC
#include <ThirdParty/glew-2.1.0/include/GL/glew.h>

#include "Engine/Window/Window.hpp"

namespace Engine
{
	class ENGINE_API OpenGLWindow : public Window
	{
	public:
		OpenGLWindow() = delete;
		OpenGLWindow(int width, int height, const char* title) noexcept;
		OpenGLWindow(OpenGLWindow const &other) = default;
		OpenGLWindow(OpenGLWindow &&other) noexcept = default;
		~OpenGLWindow() noexcept override;
	private:
		Window* windowReference;
	};
} //namespace Engine