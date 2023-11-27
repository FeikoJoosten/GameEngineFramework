#include "Engine/Scene/Scene.hpp"
#include "Engine/Components/Component.hpp" // Not including this breaks scene serialization because cereal doesn't know the component type
#include "Engine/Entity/Entity.hpp"
#include "Engine/Entity/EntitySystem.hpp"

CEREAL_REGISTER_TYPE(Engine::Scene);

namespace Engine {
	Scene::Scene() {
		EntitySystem::Get()->OnEntityRemovedEvent += Sharp::EventHandler::Bind(this, &Scene::HandleOnEntityRemovedEvent);
	}

	Scene::Scene(std::string name) : Asset(std::move(name))  {
		EntitySystem::Get()->OnEntityRemovedEvent += Sharp::EventHandler::Bind(this, &Scene::HandleOnEntityRemovedEvent);
	}

	Scene::~Scene() {
		EntitySystem::Get()->OnEntityRemovedEvent -= Sharp::EventHandler::Bind(this, &Scene::HandleOnEntityRemovedEvent);
	}

	void Scene::AddEntity(const std::shared_ptr<Entity>& entityToAdd) {
		if (std::ranges::find(entities, entityToAdd) != entities.end()) return;

		entities.push_back(entityToAdd);
		EntitySystem::Get()->AddEntity(entityToAdd);
	}

	void Scene::RemoveEntity(const std::shared_ptr<Entity>& entityToRemove) {
		std::erase_if(entities,
		              [entityToRemove](const std::shared_ptr<Entity>& entity) {
			              return entity.get() == entityToRemove.get();
		              });
		EntitySystem::Get()->RemoveEntity(entityToRemove);
	}

	bool Scene::GetIsActive() const {
		return isActive;
	}

	std::vector<std::shared_ptr<Entity>> Scene::GetAllEntities() const {
		return entities;
	}

	void Scene::SetIsActive(const bool newIsActive) {
		if (isActive == newIsActive) return;

		isActive = newIsActive;
		OnActiveStateChangedEvent(std::static_pointer_cast<Scene>(shared_from_this()), isActive);
	}

	void Scene::HandleOnEntityRemovedEvent(const std::shared_ptr<Entity> destroyedEntity) {
		RemoveEntity(destroyedEntity);
	}
}
