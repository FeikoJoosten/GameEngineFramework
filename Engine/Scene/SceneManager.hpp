#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Scene/Scene.hpp"

#include <cereal/access.hpp>
#include <memory>

namespace Engine {
	class Scene;
	class SceneImporter;

	class ENGINE_API SceneManager {
		friend std::shared_ptr<SceneManager> Engine::GetSceneManager() noexcept;
		friend cereal::access;

		std::vector<std::shared_ptr<Scene>> openScenes;
		std::shared_ptr<SceneImporter> sceneImporter;

		SceneManager() = default;

	public:
		Sharp::Event<std::shared_ptr<Scene>> OnSceneOpenedEvent;
		Sharp::Event<std::shared_ptr<Scene>> OnSceneClosedEvent;

		~SceneManager() = default;
		SceneManager(const SceneManager& other) = delete;
		SceneManager(SceneManager&& other) noexcept = delete;

		SceneManager& operator=(const SceneManager& other) = delete;
		SceneManager& operator=(SceneManager&& other) noexcept = delete;

		static std::shared_ptr<SceneManager> Get();

		std::vector<std::shared_ptr<Scene>> GetOpenScenes();

		std::shared_ptr<Scene> OpenScene(const std::string& pathInProject, const std::string& sceneName);

		void OpenScene(const std::shared_ptr<Scene>& sceneToOpen);

		void CloseScene(const std::shared_ptr<Scene>& sceneToClose);

		bool IsSceneOpened(const std::shared_ptr<Scene>& sceneToCheck);

		static std::shared_ptr<Scene> CreateScene(const std::string& sceneName = "scene", const std::string& pathInProject = "");
	};
}
