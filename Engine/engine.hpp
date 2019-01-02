#ifndef engine_HPP
#define engine_HPP

#pragma once

#include "Engine/api.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Camera/Camera.hpp"
#include "Engine/Utility/Time.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/unique_ptr.h>
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>

namespace Engine
{
class ENGINE_API Engine
{
public:
	static Window& GetWindow() noexcept;
	template<typename WindowType>
	static WindowType* GetWindow();
	static void SetWindow(eastl::unique_ptr<Window> &&window) noexcept;
	static Window& CreateAndReturnWindow(int width, int height, const char* title) noexcept;
	template<typename WindowType>
	static WindowType* CreateAndReturnWindow(int width, int height, const char* title);

	static InputManager& GetInputManager() noexcept;
	static void SetInputManager(eastl::unique_ptr<InputManager> && inputManager) noexcept;
	static InputManager& CreateAndReturnInputManager() noexcept;

	static Renderer& GetRenderer() noexcept;
	template<typename RendererType>
	static RendererType* GetRenderer();
	static void SetRenderer(eastl::unique_ptr<Renderer> && renderSystem) noexcept;
	static Renderer& CreateAndReturnRenderer() noexcept;
	template<typename RendererType>
	static RendererType* CreateAndReturnRenderer();

	static Camera& GetCamera() noexcept;

	static Time& GetTime() noexcept;

	static EntitySystem& GetEntitySystem() noexcept;

	static void Update() noexcept;
	static void Render() noexcept;

private:
	static eastl::shared_ptr<Window> window;
	static eastl::unique_ptr<InputManager> inputManager;
	static eastl::shared_ptr<Renderer> renderer;
	static eastl::unique_ptr<Camera> camera;
	static eastl::unique_ptr<Time> time;
	static eastl::unique_ptr<EntitySystem> entitySystem;
};

	template <typename WindowType>
	WindowType* Engine::GetWindow()
	{
		if (dynamic_cast<WindowType*>(window.get()))
			return dynamic_cast<WindowType*>(window.get());
		return nullptr;
	}

	template <typename WindowType>
	WindowType* Engine::CreateAndReturnWindow(int width, int height, const char* title)
	{
		SetWindow(eastl::make_unique<WindowType>(width, height, title));
		return GetWindow<WindowType>();
	}

	template <typename RendererType>
	RendererType* Engine::GetRenderer()
	{
		if (dynamic_cast<RendererType*>(renderer.get()))
			return dynamic_cast<RendererType*>(renderer.get());
		return nullptr;
	}

	template <typename RendererType>
	RendererType* Engine::CreateAndReturnRenderer()
	{
		SetRenderer(eastl::make_unique<RendererType>());
		return GetRenderer<RendererType>();
	}
} //namespace Engine

#endif // engine_HPP
