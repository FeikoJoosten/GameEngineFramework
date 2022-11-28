#include "Engine/Scene/Scene.hpp"
#include "Engine/Entity/Entity.hpp"

#include <utility>

namespace Engine {

	Scene::Scene(std::string name) : Asset(std::move(name))  {
		
	}

	Scene::~Scene() {
		for(const std::shared_ptr<Entity>& entity : entities)
			entity->OnEntityDestroyedEvent -= Sharp::EventHandler::Bind(this, &Scene::HandleOnEntityDestroyedEvent);
	}

	void Scene::AddEntity(const std::shared_ptr<Entity>& entityToAdd) {
		if (std::find(entities.begin(), entities.end(), entityToAdd) != entities.end()) return;

		if(!entityToAdd->GetGuid().isValid()) {
			const std::shared_ptr<AssetRegistry> assetRegistry = AssetManager::Get()->GetAssetRegistry();
			std::string filePath;
			std::string assetName;
			if(assetRegistry->TryGetPathForGuid(GetGuid(), filePath, assetName)) {
				const std::string entityPath = filePath + assetName + extension + "/";
				xg::Guid entityGuid;
				// TODO: Replace entityToAdd->GetName with relative path in scene
				if (!assetRegistry->TryGetGuidForPath(entityPath, entityToAdd->GetName(), entityGuid))
					assetRegistry->TryRegisterAsset(entityToAdd, entityPath);
				else assetRegistry->TryAssignGuidToAsset(entityToAdd, entityGuid);
			}
		}

		entities.push_back(entityToAdd);
		entityToAdd->OnEntityDestroyedEvent += Sharp::EventHandler::Bind(this, &Scene::HandleOnEntityDestroyedEvent);
	}

	void Scene::RemoveEntity(const std::shared_ptr<Entity>& entityToRemove) {
		entities.erase(
			std::remove_if(entities.begin(), entities.end(),
				[entityToRemove](const std::shared_ptr<Entity>& entity) {
					return entity.get() == entityToRemove.get();
				}), entities.end());

		entityToRemove->OnEntityDestroyedEvent -= Sharp::EventHandler::Bind(this, &Scene::HandleOnEntityDestroyedEvent);
	}

	bool Scene::GetIsActive() const {
		return isActive;
	}

	std::vector<std::shared_ptr<Entity>> Scene::GetAllEntities() const {
		return entities;
	}

	const std::string& Scene::GetDefaultExtension() const {
		return extension;
	}

	void Scene::SetIsActive(const bool newIsActive) {
		if (isActive == newIsActive) return;

		isActive = newIsActive;
		OnActiveStateChangedEvent(std::static_pointer_cast<Scene>(shared_from_this()), isActive);
	}

	void Scene::HandleOnEntityDestroyedEvent(const std::shared_ptr<Entity> destroyedEntity) {
		RemoveEntity(destroyedEntity);
	}
}
