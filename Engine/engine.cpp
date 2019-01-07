#include "Engine/engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/EngineImGui.hpp"

#ifdef USING_OPENGL
#include "Engine/Window/OpenGLWindow.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#endif
#ifdef USING_VULKAN
#include "Engine/Window/VulkanWindow.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"
#endif

#include <ThirdParty/cereal/include/cereal/archives/json.hpp>
#include <fstream>

namespace Engine
{
	eastl::shared_ptr<Engine> Engine::instance;
	eastl::shared_ptr<EngineImGui> engineImGui;

	eastl::weak_ptr<Window> Engine::GetWindow() noexcept
	{
		return instance->window;
	}

	eastl::weak_ptr<Window> Engine::CreateAndReturnWindow(int width, int height, const char* title) noexcept
	{
		if (instance->window == nullptr)
#ifdef USING_OPENGL
			instance->window = eastl::shared_ptr<OpenGLWindow>(new OpenGLWindow(width, height, title));
#endif
#ifdef USING_VULKAN
			instance->window = eastl::shared_ptr<VulkanWindow>(new VulkanWindow(width, height, title));
#endif
		return instance->window;
	}

	eastl::weak_ptr<InputManager> Engine::GetInputManager() noexcept
	{
		if (instance->inputManager == nullptr)
			instance->inputManager = eastl::shared_ptr<InputManager>(new InputManager());
		return instance->inputManager;
	}

	eastl::weak_ptr<Random> Engine::GetRandom()
	{
		if (instance->random == nullptr)
			instance->random = eastl::shared_ptr<Random>(new Random());
		return instance->random;
	}

	eastl::weak_ptr<Renderer> Engine::GetRenderer() noexcept
	{
		if (instance->renderer == nullptr)
		{
#ifdef USING_OPENGL
			instance->renderer = eastl::shared_ptr<OpenGLRenderer>(new OpenGLRenderer("Basic3D.vsh", "Basic3D.fsh"));
#endif
#ifdef USING_VULKAN
			instance->renderer = eastl::shared_ptr<VulkanRenderer>(new VulkanRenderer());
			GetRenderer<VulkanRenderer>().lock()->InitializeRenderer();
#endif
		}
		return instance->renderer;
	}

	eastl::weak_ptr<Camera> Engine::GetCamera() noexcept
	{
		if (instance->camera == nullptr)
			instance->camera = eastl::shared_ptr<Camera>(new Camera(
				glm::vec3(0.f, 100.f, 150.f),
				glm::vec3(0.f, -1.f, 0.f),
				60.f,
				float(GetWindow().lock()->GetHeight()) / float(GetWindow().lock()->GetHeight()),
				0.1f,
				1000.f));
		return instance->camera;
	}

	eastl::weak_ptr<Time> Engine::GetTime() noexcept
	{
		if (instance->time == nullptr)
			instance->time = eastl::shared_ptr<Time>(new Time());
		return instance->time;
	}

	eastl::weak_ptr<EntitySystem> Engine::GetEntitySystem() const noexcept
	{
		if (instance->entitySystem == nullptr)
			instance->entitySystem = eastl::shared_ptr<EntitySystem>(new EntitySystem());
		return instance->entitySystem;
	}

	eastl::weak_ptr<ResourceManager> Engine::GetResourceManager() const noexcept
	{
		if (instance->resourceManager == nullptr)
			instance->resourceManager = eastl::shared_ptr<ResourceManager>(new ResourceManager());
		return instance->resourceManager;
	}

	eastl::weak_ptr<CollisionSystem> Engine::GetCollisionSystem() const noexcept
	{
		if (instance->collisionSystem == nullptr)
			instance->collisionSystem = eastl::shared_ptr<CollisionSystem>(new CollisionSystem());
		return instance->collisionSystem;
	}

	eastl::weak_ptr<Engine> Engine::InitializeEngine(bool isPlaying) noexcept
	{
		if (instance != nullptr)
			return instance;

		instance = eastl::shared_ptr<Engine>(new Engine());
		instance->isPlaying = isPlaying;

		EngineInitializationData initializationData = instance->LoadEngineSettings();

		instance->CreateAndReturnWindow(
			initializationData.windowWidth,
			initializationData.windowHeight,
			initializationData.windowTitle.c_str());
		instance->GetRenderer();
		instance->GetInputManager();
		engineImGui = eastl::shared_ptr<EngineImGui>(new EngineImGui());
		return instance;
	}

	eastl::weak_ptr<Engine> Engine::GetEngine() noexcept
	{
		if (instance == nullptr)
			return InitializeEngine();
		return instance;
	}

	void Engine::Update() noexcept
	{
		GetTime().lock()->OnUpdateBegin();

		if (instance->isPlaying)
		{
			if (instance->entitySystem != nullptr)
				instance->entitySystem->Update();

			if (instance->collisionSystem != nullptr)
				if (instance->collisionSystem->IsRunning() == false)
					instance->collisionSystem->Start();

			if (instance->collisionSystem != nullptr)
				instance->collisionSystem->Update();
		}

		//Input manager should always update. 
		if (instance->inputManager != nullptr)
			instance->inputManager->Update();

		Render();
		GetTime().lock()->OnUpdateEnd();
	}

	void Engine::Destroy() noexcept
	{
		SaveEngineSettings();

		engineImGui.reset();
		instance->time.reset();
		instance->camera.reset();
		instance->inputManager.reset();
		instance->resourceManager.reset();
		instance->collisionSystem.reset();
		instance->entitySystem.reset();
		instance->renderer.reset();
		instance->window.reset();
		instance->random.reset();
		instance.reset();
	}

	void Engine::SetIsPlaying(bool isPlaying)
	{
		instance->isPlaying = isPlaying;
	}

	bool Engine::GetIsPlaying()
	{
		return instance->isPlaying;
	}

	EngineInitializationData Engine::LoadEngineSettings()
	{
		EngineInitializationData initializationData;
		std::ifstream inputStream("Resources/Engine/engineInitializationData.json");
		if (inputStream.good())
		{
			cereal::JSONInputArchive archive(inputStream);
			archive(initializationData);
		}

		return initializationData;
	}

	void Engine::SaveEngineSettings()
	{
		std::ofstream outputStream("Resources/Engine/engineInitializationData.json");
		cereal::JSONOutputArchive archive(outputStream);
		EngineInitializationData engineInitializationData
		{
			instance->window->GetWidth(),
			instance->window->GetHeight(),
			std::string(instance->window->GetTitle().c_str())
		};

		archive(CEREAL_NVP(engineInitializationData));
	}

	void Engine::Render() noexcept
	{
		if (instance->window != nullptr)
			instance->window->Update();

		if (instance->renderer != nullptr)
		{
			instance->renderer->RendererBegin(
				GetCamera().lock()->GetView(),
				GetCamera().lock()->GetProjection());
			instance->renderer->OnRender();

			instance->renderer->RendererEnd();
		}
	}
} // namespace Engine