#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Scene/SceneManager.hpp"

#include <memory>
#include <utility>

namespace Engine {
	int numberOfInstantiatedEntities = 0;

	EntitySystem::EntitySystem() {
		sceneManager = SceneManager::Get();
		if (!sceneManager) return; // Should never happen, but just to be safe

		sceneManager->OnSceneOpenedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnSceneOpenedEvent);
		sceneManager->OnSceneClosedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnSceneClosedEvent);
	}

	EntitySystem::~EntitySystem() {
		if (!sceneManager) return;
		
		sceneManager->OnSceneOpenedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnSceneOpenedEvent);
		sceneManager->OnSceneClosedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnSceneClosedEvent);

		for(const std::shared_ptr<Scene>& scene : sceneManager->GetOpenScenes())
			scene->OnActiveStateChangedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnSceneActiveStateChangedEvent);
	}

	std::shared_ptr<EntitySystem> EntitySystem::Get() {
		return Engine::GetEntitySystem();
	}

	void EntitySystem::RemoveAllEntities() {
		system.clear();
	}

	std::shared_ptr<Entity> EntitySystem::CreateEntity(const std::string& entityName) {
		AddEntity(std::shared_ptr<Entity>(new Entity()));

		std::shared_ptr<Entity> entityToReturn = system.back();
		entityToReturn->SetName(entityName);

		return entityToReturn;
	}

	std::vector<std::shared_ptr<Entity>> EntitySystem::GetAllEntities() const {
		return system;
	}

	void EntitySystem::AddEntity(const std::shared_ptr<Entity>& entityToAdd) {
		entityToAdd->OnComponentAddedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentAddedToEntityEvent);
		entityToAdd->OnComponentRemovedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentRemovedFromEntityEvent);
		entityToAdd->OnEntityDestroyedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::RemoveEntity);

		OnEntityAddedEvent(entityToAdd);
		system.push_back(entityToAdd);
		entityToAdd->OnEntityAddedToEntitySystem();
	}

	void EntitySystem::RemoveEntity(const std::shared_ptr<Entity> entityToRemove) {
		if (system.empty()) return;

		const std::vector<std::shared_ptr<Entity>>::iterator it = std::find(system.begin(), system.end(), entityToRemove);
		if (it == system.end())	return;

		entityToRemove->OnComponentAddedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentAddedToEntityEvent);
		entityToRemove->OnComponentRemovedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnComponentRemovedFromEntityEvent);
		entityToRemove->OnEntityDestroyedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::RemoveEntity);
		it->reset();
		system.erase(it);
	}

	void EntitySystem::Update() const {
		for (const std::shared_ptr<Entity>& entity : system) {
			if (entity->GetIsActive())
				entity->Update();
		}
	}

	void EntitySystem::HandleOnSceneOpenedEvent(const std::shared_ptr<Scene> openedScene) {
		HandleOnSceneActiveStateChangedEvent(openedScene, openedScene->GetIsActive());

		openedScene->OnActiveStateChangedEvent += Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnSceneActiveStateChangedEvent);
	}

	void EntitySystem::HandleOnSceneClosedEvent(const std::shared_ptr<Scene> closedScene) {
		HandleOnSceneActiveStateChangedEvent(closedScene, false);

		closedScene->OnActiveStateChangedEvent -= Sharp::EventHandler::Bind(this, &EntitySystem::HandleOnSceneActiveStateChangedEvent);
	}

	void EntitySystem::HandleOnSceneActiveStateChangedEvent(const std::shared_ptr<Scene> scene, const bool isActive) {
		for(const std::shared_ptr<Entity>& entity : scene->GetAllEntities()) {
			if(isActive)
				AddEntity(entity);
			else
				RemoveEntity(entity);
		}
	}

	void EntitySystem::HandleOnComponentAddedToEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> addedComponent) {
		OnComponentAddedToEntityEvent(std::move(entity), std::move(addedComponent));
	}

	void EntitySystem::HandleOnComponentRemovedFromEntityEvent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> removedComponent) {
		OnComponentRemovedFromEntityEvent(std::move(entity), std::move(removedComponent));
	}
} // namespace Engine