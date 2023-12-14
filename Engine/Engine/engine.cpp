#include "Engine/Engine/Engine.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/EngineAssetManager.hpp"
#include "Engine/Camera/CameraManager.hpp"
#include "Engine/CommandLineArguments/CommandLineArgumentsManager.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Scene/SceneManager.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Random.hpp"
#include "Engine/Utility/Time.hpp"
#include "Engine/Utility/StringUtility.hpp"

#ifdef USING_OPENGL
#include "Engine/Platform/OpenGL/Window/OpenGLWindow.hpp"
#include "Engine/Platform/OpenGL/Renderer/OpenGLRenderer.hpp"
#endif
#ifdef USING_VULKAN
#include "Engine/Platform/Vulkan/Window/VulkanWindow.hpp"
#include "Engine/Platform/Vulkan/Renderer/VulkanRenderer.hpp"
#endif

#include <tinyfiledialogs/tinyfiledialogs.h>

namespace Engine {
	std::shared_ptr<Engine> Engine::instance;
	std::shared_ptr<CommandLineArgumentsManager> Engine::commandLineArgumentsManager;

	Engine::Engine() {
		EngineAssetManager::TryLoadEngineData<Engine>(engineSettings);
		
		if(engineSettings.lastOpenedProject.empty()) {
			if(tinyfd_messageBox(
				"No project detected",
				"No last known project was detected, please select a location for a new project to be created or select the location of another project",
				"okcancel",
				"info",
				0)) {
				engineSettings.lastOpenedProject = TryGetValidProjectPath();
				engineSettings.knownProjects.push_back(engineSettings.lastOpenedProject);
				EngineAssetManager::SaveEngineData<Engine>(engineSettings);
			} else exit(0);
		}

		AssetManager::projectRoot = StringUtility::SanitizePath(engineSettings.lastOpenedProject + "/");
	}

	Engine::~Engine() {
		EngineAssetManager::SaveEngineData<Engine>(engineSettings);
	}

	std::shared_ptr<Window> Engine::GetWindow() noexcept {
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

	std::shared_ptr<InputManager> Engine::GetInputManager() noexcept {
		CreateInstance();
		if (instance->inputManager == nullptr)
			instance->inputManager = std::shared_ptr<InputManager>(new InputManager());
		return instance->inputManager;
	}

	std::shared_ptr<Renderer> Engine::GetRenderer() noexcept {
		CreateInstance();
		if (instance->renderer == nullptr) {
#ifdef USING_OPENGL
			instance->renderer = std::make_shared<OpenGLRenderer>();
#endif
#ifdef USING_VULKAN
			instance->renderer = std::shared_ptr<VulkanRenderer>(new VulkanRenderer());
			GetRenderer<VulkanRenderer>()->InitializeRenderer();
#endif
		}
		return instance->renderer;
	}

	std::shared_ptr<CameraManager> Engine::GetCameraManager() noexcept {
		CreateInstance();
		if (instance->cameraManager == nullptr) {
			instance->cameraManager = std::shared_ptr<CameraManager>(new CameraManager());
		}
		return instance->cameraManager;
	}

	std::shared_ptr<Time> Engine::GetTime() noexcept {
		CreateInstance();
		if (instance->time == nullptr)
			instance->time = std::shared_ptr<Time>(new Time());
		return instance->time;
	}

	std::shared_ptr<EntitySystem> Engine::GetEntitySystem() noexcept {
		CreateInstance();
		if (instance->entitySystem == nullptr)
			instance->entitySystem = std::shared_ptr<EntitySystem>(new EntitySystem());
		return instance->entitySystem;
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

	std::shared_ptr<EngineAssetManager> Engine::GetEngineAssetManager() noexcept {
		CreateInstance();
		if (instance->engineAssetManager == nullptr)
			instance->engineAssetManager = std::shared_ptr<EngineAssetManager>(new EngineAssetManager());
		return instance->engineAssetManager;
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

	std::string Engine::TryGetValidProjectPath() {
		const std::string pathToReturn = tinyfd_selectFolderDialog("Select Project Location", "");
		if(pathToReturn.empty()) {
			if(tinyfd_messageBox(
				"No path selected",
				"You haven't selected a path. If you wish to quit, select cancel or press OK to select another path.",
				"okcancel",
				"error",
				0))
				return TryGetValidProjectPath();

			exit(0);
		} //else if() TODO: Verify folder location, empty folder or project structure detected?

		return StringUtility::SanitizePath(pathToReturn);
	}
}