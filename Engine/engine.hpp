#pragma once

#include "Engine/api.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Camera/Camera.hpp"
#include "Engine/Utility/Time.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Resources/ResourceManager.hpp"
#include "Engine/Collision/CollisionSystem.hpp"
#include "Engine/Utility/Random.hpp"

#include <ThirdParty/cereal/include/cereal/cereal.hpp>

namespace Engine
{
	struct EngineInitializationData
	{
		/// <summary>
		/// WindowWidth defines the width of the engine window.
		/// Default is 640.
		/// </summary>
		int windowWidth = 640;
		/// <summary>
		/// WindowHeight defines the height of the engine window.
		/// Default is 480.
		/// </summary>
		int windowHeight = 480;
		/// <summary>
		/// Window Title defines the title of the engine window.
		/// Default is "".
		/// </summary>
		std::string windowTitle = "";

		//Need to keep the lowecase s due to the way cereal works
		template <class Archive>
		void serialize(Archive& ar);
	};

	class ENGINE_API Engine
	{
	public:
		/// <summary>
		/// This method allows you to initialize the Engine.
		/// </summary>
		/// <param name="isPlaying">This value will defined if the engine should start in play mode. True by default</param>
		/// <returns>Returns a weak pointer of the Engine.</returns>
		static eastl::weak_ptr<Engine> InitializeEngine(bool isPlaying = true) noexcept;

		/// <summary>
		/// This method allows you to get the instance of the Engine. 
		/// This method will automatically initialize the Engine if it hasn't been initialized yet.
		/// </summary>
		/// <returns>Returns a weak pointer of the Engine instance.</returns>
		static eastl::weak_ptr<Engine> GetEngine() noexcept;

		/// <summary>
		/// This method allows you to get a weak pointer of the Window. 
		/// This method will automatically create the engine for you based on the defined renderer type.
		/// </summary>
		/// <returns>Returns a weak pointer of the defined renderer type.</returns>
		eastl::weak_ptr<Window> GetWindow() noexcept;

		template<typename WindowType>
		/// <summary>
		/// This method allows you to get a weak pointer of the Window based on the defined WindowType.
		/// </summary>
		/// <returns>Returns a weak pointer of the defined renderer type if possible. 
		/// Will return an expired weak pointer if it's not the initialized WindowType.</returns>
		eastl::weak_ptr<WindowType> GetWindow();

		/// <summary>
		/// This method will initialize the window class for you, based on the defined renderer type.
		/// </summary>
		/// <param name="width">The width of the window.</param>
		/// <param name="height">The height of the window.</param>
		/// <param name="title">The title of the window.</param>
		/// <returns>Returns a weak pointer of the window.</returns>
		eastl::weak_ptr<Window> CreateAndReturnWindow(int width, int height, const char* title) noexcept;

		/// <summary>
		/// This method allows you to get a weak pointer of the input manager. 
		/// If it hasn't been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns the input manager as a weak pointer.</returns>
		eastl::weak_ptr<InputManager> GetInputManager() noexcept;

		/// <summary>
		/// This method allows you to get a weak pointer of the renderer. 
		/// If it hasn't been defined yet, it'll be created for you based on the defined renderer type.
		/// </summary>
		/// <returns>Returns the renderer as a weak pointer.</returns>
		eastl::weak_ptr<Renderer> GetRenderer() noexcept;

		template<typename RendererType>
		/// <summary>
		/// This method allows you to get a weak pointer of the renderer based on the defined RendererType. 
		/// If it hasn't been defined yet, it'll be created for you based on the defined renderer type.
		/// </summary>
		/// <returns>Returns a weak pointer of the renderer if possible. 
		/// Otherwise it'll return an expired weak pointer.</returns>
		eastl::weak_ptr<RendererType> GetRenderer();

		/// <summary>
		/// This method allows you to get a weak pointer of the main camera object. 
		/// If it hasn't been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns the camera as a weak pointer.</returns>
		eastl::weak_ptr<Camera> GetCamera() noexcept;

		/// <summary>
		/// This method allows you to get a weak pointer of the time object. 
		/// If it hasn't been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a weak pointer of the time object.</returns>
		eastl::weak_ptr<Time> GetTime() noexcept;

		/// <summary>
		/// This method allows you to get a weak pointer of the entity system. 
		/// If it hasn't been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a weak pointer of the entity system.</returns>
		eastl::weak_ptr<EntitySystem> GetEntitySystem() const noexcept;

		/// <summary>
		/// This method allows you to get a weak pointer of the resource manager. 
		/// If it hasn't been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a weak pointer of the resource manager.</returns>
		eastl::weak_ptr<ResourceManager> GetResourceManager() const noexcept;

		/// <summary>
		/// This method allows you to get a weak pointer of the collision system. If it hasn't been defined yet, it'll be created for you.
		/// </summary>
		/// <returns>Returns a weak pointer of the collision system.</returns>
		eastl::weak_ptr<CollisionSystem> GetCollisionSystem() const noexcept;

		/// <summary>
		/// this method allows you to get a weak pointer of the Random class object
		/// </summary>
		/// <returns>Returns a weak pointer to Random class object</returns>
		eastl::weak_ptr<Random> GetRandom();

		/// <summary>
		/// The general update method of the entire engine system. 
		/// Call this from your main loop.
		/// </summary>
		void Update() noexcept;

		/// <summary>
		/// This method destroys the entire engine for you. 
		/// Call this method after your main loop is finished.
		/// </summary>
		void Destroy() noexcept;

		/// <summary>
		/// This method allows you to pause or play the game based on the assigned value.
		/// </summary>
		/// <param name="isPlaying">This wil change the engine to pause or play mode based on your value.</param>
		void SetIsPlaying(bool isPlaying);
		/// <summary>
		/// This method allows you to get the current status of the engine.
		/// </summary>
		/// <returns>Will return true if the engine is in play mode.</returns>
		bool GetIsPlaying();

	private:
		explicit Engine() = default;
	public:
		~Engine() = default;
	private:

		EngineInitializationData LoadEngineSettings();
		void SaveEngineSettings();

		void Render() noexcept;

		static eastl::shared_ptr<Engine> instance;
		eastl::shared_ptr<Window> window;
		eastl::shared_ptr<InputManager> inputManager;
		eastl::shared_ptr<Renderer> renderer;
		eastl::shared_ptr<Camera> camera;
		eastl::shared_ptr<Time> time;
		eastl::shared_ptr<EntitySystem> entitySystem;
		eastl::shared_ptr<ResourceManager> resourceManager;
		eastl::shared_ptr<CollisionSystem> collisionSystem;
		eastl::shared_ptr<Random> random;
		bool isPlaying;
	};

	template <class Archive>
	void EngineInitializationData::serialize(Archive& ar)
	{
		ar(CEREAL_NVP(windowWidth),
			CEREAL_NVP(windowHeight),
			CEREAL_NVP(windowTitle));
	}

	template <typename WindowType>
	eastl::weak_ptr<WindowType> Engine::GetWindow()
	{
		if (instance->window == nullptr)
			GetWindow();

		if (dynamic_cast<WindowType*>(instance->window.get()))
			return eastl::static_pointer_cast<WindowType>(instance->window);
		return eastl::weak_ptr<WindowType>();
	}

	template <typename RendererType>
	eastl::weak_ptr<RendererType> Engine::GetRenderer()
	{
		if (instance->renderer == nullptr)
			GetRenderer();

		if (dynamic_cast<RendererType*>(instance->renderer.get()))
			return eastl::static_pointer_cast<RendererType>(instance->renderer);
		return eastl::shared_ptr<RendererType>();
	}

} //namespace Engine