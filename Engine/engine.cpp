#include "Engine/engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/EngineImGui.hpp"
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>

#if RENDERER == 1
#include "Engine/Window/OpenGLWindow.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#elif RENDERER == 2
#include "Engine/Window/VulkanWindow.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"
#endif

namespace Engine
{
	eastl::shared_ptr<Window> Engine::window;
	eastl::unique_ptr<InputManager> Engine::inputManager;
	eastl::shared_ptr<Renderer> Engine::renderer;
	eastl::unique_ptr<Camera> Engine::camera;
	eastl::unique_ptr<Time> Engine::time;
	eastl::unique_ptr<EntitySystem> Engine::entitySystem;
	eastl::unique_ptr<EngineImGui> engineImGui;
	bool showMenuBar = false;

	Window& Engine::GetWindow() noexcept
	{
		return *window;
	}

	void Engine::SetWindow(eastl::unique_ptr<Window> &&window) noexcept
	{
		Engine::window = eastl::move(window);
	}

	Window& Engine::CreateAndReturnWindow(int width, int height, const char* title) noexcept
	{
		if (window == nullptr)
#if RENDERER == 1
			window = eastl::make_unique<OpenGLWindow>(width, height, title);
#elif RENDERER == 2
			window = eastl::make_unique<VulkanWindow>(width, height, title);
#else
			window = eastl::make_unique<Window>(width, height, title);
#endif
		return *window;
	}

	InputManager& Engine::GetInputManager() noexcept
	{
		return *inputManager;
	}

	void Engine::SetInputManager(eastl::unique_ptr<InputManager>&& inputManager) noexcept
	{
		Engine::inputManager = eastl::move(inputManager);
	}

	InputManager& Engine::CreateAndReturnInputManager() noexcept
	{
		if (inputManager == nullptr)
			SetInputManager(eastl::make_unique<InputManager>());
		return *inputManager;
	}

	Renderer& Engine::GetRenderer() noexcept
	{
		return *renderer;
	}

	void Engine::SetRenderer(eastl::unique_ptr<Renderer> && renderSystem) noexcept
	{
		Engine::renderer = eastl::move(renderSystem);
	}

	Renderer& Engine::CreateAndReturnRenderer() noexcept
	{
		if (renderer == nullptr)
#if RENDERER == 1
			renderer = eastl::make_unique<OpenGLRenderer>();
#elif RENDERER == 2
			renderer = eastl::make_unique<VulkanRenderer>();
#else
			renderer = eastl::make_unique<Renderer>();
#endif
		return *renderer;
	}

	Camera& Engine::GetCamera() noexcept
	{
		if (camera == nullptr)
		{
			camera = eastl::make_unique<Camera>(glm::vec3(0.f, 100.f, 150.f), glm::vec3(0.f, -1.f, 0.f), 60.f, float(GetWindow().GetHeight()) / float(GetWindow().GetHeight()), 0.1f, 1000.f);
		}
		return *camera;
	}

	Time& Engine::GetTime() noexcept
	{
		if (Engine::time == nullptr)
			Engine::time = eastl::make_unique<Time>();
		return *Engine::time;
	}

	EntitySystem& Engine::GetEntitySystem() noexcept
	{
		if (Engine::entitySystem == nullptr)
			Engine::entitySystem = eastl::make_unique<EntitySystem>();
		return *Engine::entitySystem;
	}

	void Engine::Update() noexcept
	{
		GetTime().OnUpdateBegin();

		if (Engine::entitySystem != nullptr)
			Engine::entitySystem->Update();

		if (inputManager != nullptr)
			inputManager->Update();

		Render();

		GetTime().OnUpdateEnd();
	}

	float devMenuCooldown = 0.f;

	void Engine::Render() noexcept
	{
		if (window != nullptr)
			window->Update();

		if (renderer != nullptr)
		{
			if (engineImGui == nullptr)
				engineImGui = eastl::make_unique<EngineImGui>();

			engineImGui->CameraMovement();

			renderer->RendererBegin(GetCamera().GetView(), GetCamera().GetProjection());
			renderer->OnRender();

			const gainput::InputDevice* keyboard = GetInputManager().GetDevice(GetInputManager().GetKeyboardID());
			if(keyboard->GetBool(gainput::KeyCtrlL) && keyboard->GetBool(gainput::KeyShiftL) && keyboard->GetBool(gainput::KeyE))
			{
				if(devMenuCooldown < 0)
				{
					showMenuBar = !showMenuBar;
					devMenuCooldown = 0.25f;
				}
			}
			if (devMenuCooldown >= 0)
				devMenuCooldown -= Engine::time->GetDeltaTime();

			if (showMenuBar)
			{
				engineImGui->DevMenu();
			}

			renderer->RendererEnd();
		}
	}
} // namespace Engine
