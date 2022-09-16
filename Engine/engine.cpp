#include "Engine/Engine.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Camera/CameraManager.hpp"
#include "Engine/Collision/CollisionSystem.hpp"
#include "Engine/CommandLineArguments/CommandLineArgumentsManager.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Resources/ResourceManager.hpp"
#include "Engine/Scene/SceneManager.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/EngineImGui.hpp"
#include "Engine/Utility/Random.hpp"
#include "Engine/Utility/Time.hpp"

#ifdef USING_OPENGL
#include "Engine/Window/OpenGLWindow.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#endif
#ifdef USING_VULKAN
#include "Engine/Window/VulkanWindow.hpp"
#include "Engine/Renderer/VulkanRenderer.hpp"
#endif

#include <fstream>
#include <cereal/archives/json.hpp>

namespace Engine
{
	std::shared_ptr<Engine> Engine::instance;
	std::shared_ptr<CommandLineArgumentsManager> Engine::commandLineArgumentsManager;

	std::shared_ptr<Window> Engine::GetWindow() noexcept
	{
		CreateInstance();
		if (instance->window == nullptr)
#ifdef USING_OPENGL
			instance->window = std::shared_ptr<OpenGLWindow>(new OpenGLWindow());
#endif
#ifdef USING_VULKAN
		instance->window = std::shared_ptr<VulkanWindow>(new VulkanWindow());
#endif
		return instance->window;
	}

	std::shared_ptr<InputManager> Engine::GetInputManager() noexcept
	{
		CreateInstance();
		if (instance->inputManager == nullptr)
			instance->inputManager = std::shared_ptr<InputManager>(new InputManager());
		return instance->inputManager;
	}

	std::shared_ptr<Renderer> Engine::GetRenderer() noexcept
	{
		CreateInstance();
		if (instance->renderer == nullptr)
		{
#ifdef USING_OPENGL
			instance->renderer = std::shared_ptr<OpenGLRenderer>(new OpenGLRenderer("Basic3D.vert", "Basic3D.frag"));
#endif
#ifdef USING_VULKAN
			instance->renderer = std::shared_ptr<VulkanRenderer>(new VulkanRenderer());
			GetRenderer<VulkanRenderer>()->InitializeRenderer();
#endif
		}
		return instance->renderer;
	}

	std::shared_ptr<CameraManager> Engine::GetCameraManager() noexcept
	{
		CreateInstance();
		if(instance->cameraManager == nullptr) {
			instance->cameraManager = std::shared_ptr<CameraManager>(new CameraManager());
			/*if(instance->cameraEntity.expired())
				instance->cameraEntity = GetEntitySystem()->CreateEntity("Editor Camera");

			const std::shared_ptr<Entity> lockedEntity = instance->cameraEntity.lock();
			instance->cameraManager = lockedEntity->GetComponent<CameraComponent>();
			if(instance->cameraManager == nullptr) {
				const std::shared_ptr<Window> lockedWindow = GetWindow();
				instance->cameraManager = lockedEntity->AddComponent<CameraComponent>(
					glm::vec3(0.f, 0.f, 0.f),
					glm::vec3(0.f, 0.f, 0.f),
					60.f,
					static_cast<float>(lockedWindow->GetHeight()) / static_cast<float>(lockedWindow->GetWidth()),
					0.1f,
					1000.f);
			}
			const std::shared_ptr<TransformComponent> transformComponent = lockedEntity->GetComponent<TransformComponent>();
			if(!transformComponent)
				lockedEntity->AddComponent<TransformComponent>();*/
		}
		return instance->cameraManager;
	}

	std::shared_ptr<Time> Engine::GetTime() noexcept
	{
		CreateInstance();
		if (instance->time == nullptr)
			instance->time = std::shared_ptr<Time>(new Time());
		return instance->time;
	}

	std::shared_ptr<EntitySystem> Engine::GetEntitySystem() noexcept
	{
		CreateInstance();
		if (instance->entitySystem == nullptr)
			instance->entitySystem = std::shared_ptr<EntitySystem>(new EntitySystem());
		return instance->entitySystem;
	}

	std::shared_ptr<ResourceManager> Engine::GetResourceManager() noexcept
	{
		CreateInstance();
		if (instance->resourceManager == nullptr)
			instance->resourceManager = std::shared_ptr<ResourceManager>(new ResourceManager());
		return instance->resourceManager;
	}

	std::shared_ptr<CollisionSystem> Engine::GetCollisionSystem() noexcept
	{
		CreateInstance();
		if (instance->collisionSystem == nullptr)
			instance->collisionSystem = std::shared_ptr<CollisionSystem>(new CollisionSystem());
		return instance->collisionSystem;
	}

	std::shared_ptr<Random> Engine::GetRandom() noexcept {
		CreateInstance();
		if (instance->random == nullptr)
			instance->random = std::shared_ptr<Random>(new Random());
		return instance->random;
	}

	std::shared_ptr<SceneManager> Engine::GetSceneManager() noexcept {
		CreateInstance();
		if (instance->sceneManager == nullptr)
			instance->sceneManager = std::shared_ptr<SceneManager>(new SceneManager());
		return instance->sceneManager;
	}

	std::shared_ptr<AssetManager> Engine::GetAssetManager() noexcept {
		CreateInstance();
		if (instance->assetManager == nullptr)
			instance->assetManager = std::shared_ptr<AssetManager>(new AssetManager());
		return instance->assetManager;
	}

	std::shared_ptr<CommandLineArgumentsManager> Engine::GetOrCreateCommandLineArgumentsManager(std::vector<std::string> commandLineArguments) noexcept {
		if (commandLineArgumentsManager == nullptr)
			commandLineArgumentsManager = std::shared_ptr<CommandLineArgumentsManager>(new CommandLineArgumentsManager(std::move(commandLineArguments)));
		return commandLineArgumentsManager;
	}

	void Engine::CreateInstance() noexcept {

		if (instance != nullptr)
			return;

		instance = std::shared_ptr<Engine>(new Engine());
	}
} // namespace Engine