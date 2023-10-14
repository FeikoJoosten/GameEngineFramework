#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Event.hpp"

#include <cereal/access.hpp>

namespace Engine {

	class ENGINE_API Scene final : public Asset {
		friend cereal::access;
		friend class SceneAssetImporter;
		friend class SceneManager;

		bool isActive = true;
		std::vector<std::shared_ptr<Entity>> entities {};

		Scene();
		explicit Scene(std::string name);

	public:
		Sharp::Event<std::shared_ptr<Scene>, bool> OnActiveStateChangedEvent;

		virtual ~Scene() override;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) noexcept = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) noexcept = delete;

		void AddEntity(const std::shared_ptr<Entity>& entityToAdd);

		void RemoveEntity(const std::shared_ptr<Entity>& entityToRemove);

		[[nodiscard]] bool GetIsActive() const;

		[[nodiscard]] std::vector<std::shared_ptr<Entity>> GetAllEntities() const;

		void SetIsActive(bool newIsActive);

	private:
		
		void HandleOnEntityRemovedEvent(std::shared_ptr<Entity> destroyedEntity);

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void Scene::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(isActive),
			CEREAL_NVP(entities)
		);
	}
}
