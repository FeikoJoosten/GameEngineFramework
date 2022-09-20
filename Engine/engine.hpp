#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Engine {
	class AssetManager;
	class CameraManager;
	class CollisionSystem;
	class CommandLineArgumentsManager;
	class EngineAssetManager;
	class EntitySystem;
	class InputManager;
	class Renderer;
	class Random;
	class ResourceManager;
	class SceneManager;
	class Time;
	class Window;

	class Engine {
		friend class Application;

		explicit Engine() = default;
	public:
		Engine(const Engine& other) = delete;
		Engine(Engine&& other) noexcept = delete;
		~Engine() = default;

		Engine& operator=(const Engine& other) = delete;
		Engine& operator=(Engine&& other) noexcept = delete;

		/// <summary>
		/// This method allows you to get a shared pointer of the Window. 
		/// This method will automatically create the engine for you based on the defined renderer type.
		/// </summary>
		/// <returns>Returns a shared pointer of the defined renderer type.</returns>
		static std::shared_ptr<Window> GetWindow() noexcept;

		template<typename WindowType>
		/// <summary>
		/// This method allows you to get a weak pointer of the Window based on the defined WindowType.
		/// If it has not been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a shared pointer of the defined renderer type if possible.</returns>
		static std::shared_ptr<WindowType> GetWindow();

		/// <summary>
		/// This method allows you to get a shared pointer of the input manager. 
		/// If it has not been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns the input manager as a shared pointer.</returns>
		static std::shared_ptr<InputManager> GetInputManager() noexcept;

		/// <summary>
		/// This method allows you to get a shared pointer of the renderer. 
		/// If it has not been defined yet, it'll be created for you based on the defined renderer type.
		/// </summary>
		/// <returns>Returns the renderer as a shared pointer.</returns>
		static std::shared_ptr<Renderer> GetRenderer() noexcept;

		template<typename RendererType>
		/// <summary>
		/// This method allows you to get a shared pointer of the renderer based on the defined RendererType. 
		/// If it has not been defined yet, it'll be created for you based on the defined renderer type.
		/// </summary>
		/// <returns>Returns a shared pointer of the renderer if possible. 
		/// Otherwise it'll return a shared pointer.</returns>
		static std::shared_ptr<RendererType> GetRenderer();

		/// <summary>
		/// This method allows you to get a shared pointer of the camera manager. 
		/// If it has not been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns the camera manager as a shared pointer.</returns>
		static std::shared_ptr<CameraManager> GetCameraManager() noexcept;

		/// <summary>
		/// This method allows you to get a shared pointer of the time object. 
		/// If it has not been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a shared pointer of the time object.</returns>
		static std::shared_ptr<Time> GetTime() noexcept;

		/// <summary>
		/// This method allows you to get a shared pointer of the entity system. 
		/// If it has not been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a shared pointer of the entity system.</returns>
		static std::shared_ptr<EntitySystem> GetEntitySystem() noexcept;

		/// <summary>
		/// This method allows you to get a shared pointer of the resource manager. 
		/// If it has not been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a shared pointer of the resource manager.</returns>
		static std::shared_ptr<ResourceManager> GetResourceManager() noexcept;

		/// <summary>
		/// This method allows you to get a shared pointer of the collision system.
		/// If it has not been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a shared pointer of the collision system.</returns>
		static std::shared_ptr<CollisionSystem> GetCollisionSystem() noexcept;

		/// <summary>
		/// this method allows you to get a shared pointer of the Random class object
		/// If it has not been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a shared pointer to Random class object</returns>
		static std::shared_ptr<Random> GetRandom() noexcept;

		/// <summary>
		///	This method allows you to get a shared pointer of the SceneManager
		/// If it has not been defined yet, it'll be created for you.
		///	</summary>
		///	<returns>Returns a shared pointer to SceneManager</returns>
		static std::shared_ptr<SceneManager> GetSceneManager() noexcept;

		/// <summary>
		///	This method allows you to get a shared pointer of the AssetManager
		/// If it has not been defined yet, it'll be created for you.
		///	</summary>
		///	<returns>Returns a shared pointer to AssetManager</returns>
		static std::shared_ptr<AssetManager> GetAssetManager() noexcept;

		/// <summary>
		///	This method allows you to get a shared pointer of the EngineAssetManager
		/// If it has not been defined yet, it'll be created for you.
		///	</summary>
		///	<returns>Returns a shared pointer to EngineAssetManager</returns>
		static std::shared_ptr<EngineAssetManager> GetEngineAssetManager() noexcept;

		/// <summary>
		///	This method allows you to get a shared pointer of the CommandLineArgumentsManager
		/// If it has not been defined yet, it'll be created for you.
		///	</summary>
		///	<returns>Returns a shared pointer to CommandLineArgumentsManager</returns>
		static std::shared_ptr<CommandLineArgumentsManager> GetOrCreateCommandLineArgumentsManager(std::vector<std::string> commandLineArguments = {}) noexcept;

	private:

		static std::shared_ptr<Engine> instance;
		std::shared_ptr<Window> window;
		std::shared_ptr<InputManager> inputManager;
		std::shared_ptr<Renderer> renderer;
		std::shared_ptr<CameraManager> cameraManager;
		std::shared_ptr<Time> time;
		std::shared_ptr<EntitySystem> entitySystem;
		std::shared_ptr<ResourceManager> resourceManager;
		std::shared_ptr<CollisionSystem> collisionSystem;
		std::shared_ptr<Random> random;
		std::shared_ptr<SceneManager> sceneManager;
		std::shared_ptr<AssetManager> assetManager;
		std::shared_ptr<EngineAssetManager> engineAssetManager;
		static std::shared_ptr<CommandLineArgumentsManager> commandLineArgumentsManager;

		static void CreateInstance() noexcept;
	};

	template <typename WindowType>
	std::shared_ptr<WindowType> Engine::GetWindow() {
		CreateInstance();
		std::shared_ptr<Window> windowInstance = instance->window;
		if (!windowInstance)
			windowInstance = GetWindow();

		return std::dynamic_pointer_cast<WindowType>(windowInstance);
	}

	template <typename RendererType>
	std::shared_ptr<RendererType> Engine::GetRenderer() {
		CreateInstance();
		std::shared_ptr<Renderer> rendererInstance = instance->renderer;
		if (!rendererInstance)
			rendererInstance = GetRenderer();

		return std::dynamic_pointer_cast<RendererType>(rendererInstance);
	}

} //namespace Engine