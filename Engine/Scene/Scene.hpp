#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/SceneImporter.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Event.hpp"

#include <cereal/access.hpp>

namespace Engine {

	class ENGINE_API Scene final : public Asset {
		friend cereal::access;
		friend class SceneManager;
		friend bool SceneImporter::SupportsFileExtension(const std::string&);

		bool isActive = true;
		std::vector<std::shared_ptr<Entity>> entities {};
		static inline std::string extension = ".scene";

		Scene() = default;
		explicit Scene(std::string name);

	public:
		Sharp::Event<std::shared_ptr<Scene>, bool> OnActiveStateChangedEvent;

		Scene(const Scene& other) = delete;
		Scene(Scene&& other) noexcept = delete;

		virtual ~Scene() override;

		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) noexcept = delete;

		void AddEntity(const std::shared_ptr<Entity>& entityToAdd);

		void RemoveEntity(const std::shared_ptr<Entity>& entityToRemove);

		[[nodiscard]] bool GetIsActive() const;

		[[nodiscard]] std::vector<std::shared_ptr<Entity>> GetAllEntities() const;

		[[nodiscard]] virtual const std::string& GetDefaultExtension() const override;

		void SetIsActive(bool newIsActive);

	private:
		
		void HandleOnEntityDestroyedEvent(std::shared_ptr<Entity> destroyedEntity);

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void Scene::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::virtual_base_class<Asset>(this)),
			CEREAL_NVP(isActive),
			CEREAL_NVP(entities)
		);
	}
}

CEREAL_REGISTER_TYPE(Engine::Scene);
