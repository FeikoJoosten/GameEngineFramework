#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Engine/Engine.hpp"
#define GLEW_STATIC
#include "Engine/Window/Window.hpp"

namespace Engine {
	/// <summary>
	/// This object stores any information regarding the created GLFW OpenGLWindow.
	/// </summary>
	class ENGINE_API OpenGLWindow : public Window {
		friend std::shared_ptr<Window> Engine::GetWindow() noexcept;

		OpenGLWindow() noexcept;
	public:
		OpenGLWindow(const OpenGLWindow& other) = delete;
		OpenGLWindow(OpenGLWindow&& other) noexcept = delete;
		virtual ~OpenGLWindow() noexcept override = default;

		OpenGLWindow& operator=(const OpenGLWindow& other) = delete;
		OpenGLWindow& operator=(OpenGLWindow&& other) noexcept = delete;
	};
} //namespace Engine
#endif