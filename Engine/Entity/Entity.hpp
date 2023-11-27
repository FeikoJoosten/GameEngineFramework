#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Event.hpp"

#include <cereal/types/base_class.hpp>
#include <cereal/types/vector.hpp>

namespace Engine {

	class Component;

	/// <summary>
	/// This object is a holder object for components.
	/// </summary>
	class ENGINE_API Entity final : public Asset {
		friend cereal::access;
		// Need to friend the entirety of EntitySystem as EntitySystem::AddEntity is a private function
		friend class EntitySystem;
		friend class EntityAssetImporter;

		bool isActive = true;
		std::vector<std::shared_ptr<Component>> components {};

		Entity() = default;

	public:
		Sharp::Event<std::shared_ptr<Entity>, std::shared_ptr<Component>> OnComponentAddedEvent;
		Sharp::Event<std::shared_ptr<Entity>, std::shared_ptr<Component>> OnComponentRemovedEvent;
		Sharp::Event<std::shared_ptr<Entity>, bool> OnActiveStateChangedEvent;

		explicit Entity(std::string name);
		Entity(const Entity& other) = delete;
		Entity(Entity&& other) = delete;
		Entity& operator=(const Entity& other) = delete;
		Entity& operator=(Entity&& other) = delete;

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get the first component of the given type.
		/// </summary>
		/// <returns>Returns the first component of the given type as a shared pointer.</returns>
		[[nodiscard]] std::shared_ptr<ComponentType> GetComponent();

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get all of the components of the given type.
		/// </summary>
		/// <returns>Returns the all of the components of the given type as a shared pointer.</returns>
		[[nodiscard]] std::vector<std::shared_ptr<ComponentType>> GetComponents();

		/// <summary>
		/// Returns all the components of this entity.
		/// </summary>
		/// <returns>Returns a vector of all the components of this entity.</returns>
		[[nodiscard]] std::vector<std::shared_ptr<Component>> GetAllComponents() const;

		template <class ComponentType>
		/// <summary>
		/// Allows you to create a component of the given type.
		/// </summary>
		/// <returns>Returns the just created component of the given type as a shared pointer.</returns>
		std::shared_ptr<ComponentType> AddComponent();

		template <class ComponentType>
		/// <summary>
		/// Adds X amount of components of the given type.
		/// </summary>
		/// <param name="count">The amount of components you want to add of this given type.</param>
		/// <returns>Returns a vector of the just created components of the given type as a vector of shared pointers.</returns>
		std::vector<std::shared_ptr<ComponentType>> AddComponents(size_t count);

		template <typename ComponentType>
		/// <summary>
		/// Removes X amount of components of the given type.
		/// </summary>
		/// <param name="amountToRemove">Will remove 1 component by default, can be change optionally.</param>
		void RemoveComponent(size_t amountToRemove = 1);

		template <typename ComponentType>
		/// <summary>
		/// Deletes all the components of the given type.
		/// </summary>
		void RemoveAllComponents();

		/// <summary>
		/// The general update method of the Entity class.
		/// </summary>
		void Update() const;

		/// <summary>
		/// This method will return the active state of this entity.
		/// </summary>
		/// <returns>Returns true if this entity is active.</returns>
		[[nodiscard]] bool GetIsActive() const;

		/// <summary>
		/// This method will allow you to change the active state of this entity.
		/// </summary>
		/// <param name="newIsActive">This value will define if this entity will be active or not.</param>
		void SetIsActive(bool newIsActive);

	private:

		void OnEntityAddedToEntitySystem();

		void InitializeComponent(const std::shared_ptr<Component>& componentToInitialize);

		void OnComponentAdded(const std::shared_ptr<Component>& addedComponent) const;

		void OnComponentRemoved(const std::shared_ptr<Component>& removedComponent) const;

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <typename ComponentType>
	std::shared_ptr<ComponentType> Entity::GetComponent() {
		for (const std::shared_ptr<Component> component : components) {
			if (const std::shared_ptr<ComponentType> componentToReturn = std::dynamic_pointer_cast<ComponentType>(component))
				return componentToReturn;
		}
		return nullptr;
	}

	template <typename ComponentType>
	std::vector<std::shared_ptr<ComponentType>> Entity::GetComponents() {
		std::vector<std::shared_ptr<ComponentType>> componentsToReturn;

		for (const std::shared_ptr<Component> component : components) {
			if (const std::shared_ptr<ComponentType> componentToReturn = std::dynamic_pointer_cast<ComponentType>(component))
				componentsToReturn.push_back(componentToReturn);
		}

		return componentsToReturn;
	}

	template <class ComponentType>
	std::shared_ptr<ComponentType> Entity::AddComponent() {
		components.push_back(std::shared_ptr<ComponentType>(new ComponentType()));

		const std::shared_ptr<Component> componentToReturn = components.back();
		InitializeComponent(componentToReturn);

		OnComponentAddedEvent(std::static_pointer_cast<Entity>(shared_from_this()), componentToReturn);
		return std::static_pointer_cast<ComponentType>(componentToReturn);
	}

	template <class ComponentType>
	std::vector<std::shared_ptr<ComponentType>> Entity::AddComponents(const size_t count) {
		std::vector<std::shared_ptr<ComponentType>> componentsToReturn;

		for (size_t i = 0; i < count; ++i)
			componentsToReturn.push_back(AddComponent<ComponentType>());
		return componentsToReturn;
	}

	template <typename ComponentType>
	void Entity::RemoveComponent(size_t amountToRemove) {
		if (amountToRemove > components.size())
			amountToRemove = components.size();

		const std::vector<std::shared_ptr<Component>>::iterator componentsBegin = components.begin();
		for (size_t i = components.size() - 1; i != -1; --i) {
			if (amountToRemove == 0)
				break;

			if (std::shared_ptr<ComponentType> componentToRemove = std::dynamic_pointer_cast<ComponentType>(components[i])) {
				components.erase(componentsBegin + i);
				OnComponentRemoved(componentToRemove);

				for (const std::shared_ptr<Component>& component : components)
					componentToRemove->OnComponentRemoved(component);

				OnComponentAddedEvent(std::static_pointer_cast<Entity>(shared_from_this()), componentToRemove);
			}
			amountToRemove--;
		}
	}

	template <typename ComponentType>
	void Entity::RemoveAllComponents() {
		RemoveComponent<ComponentType>(components.size());
	}

	template<class Archive>
	void Entity::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(components),
			CEREAL_NVP(isActive)
		);
	}
}
