#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Scene/Scene.hpp"

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <memory>


namespace Engine {
	class Scene;

	class ENGINE_API SceneManager {
		friend std::shared_ptr<SceneManager> Engine::GetSceneManager() noexcept;
		friend cereal::access;

		SceneManager();

	public:
		SceneManager(const SceneManager& other) = delete;
		SceneManager(SceneManager&& other) noexcept = delete;
		~SceneManager();

		SceneManager& operator=(const SceneManager& other) = delete;
		SceneManager& operator=(SceneManager&& other) noexcept = delete;

		static std::shared_ptr<SceneManager> Get();

		std::shared_ptr<Scene> GetActiveScene();

		void OpenScene(const std::string& path);

		void OpenScene(std::shared_ptr<Scene> scene);

	private:
		std::shared_ptr<Scene> activeScene;

		template <class Archive>
		void Save(Archive& archive);

		template <class Archive>
		void Load(Archive& archive);
	};

	template <class Archive>
	void SceneManager::Save(Archive& archive) {
		archive(
			cereal::make_nvp("lastActiveScene", activeScene->GetPath())
		);
	}

	template <class Archive> void SceneManager::Load(Archive& archive) {
		std::string lastActiveScenePath;
		archive(lastActiveScenePath);

		OpenScene(lastActiveScenePath);
	}
}
