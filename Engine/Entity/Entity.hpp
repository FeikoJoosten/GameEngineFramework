#pragma once
#include "Engine/Api.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Utility/Event.hpp"

namespace Engine {
	/// <summary>
	/// This object is a holder object for components. NOTE: only the EntitySystem is allowed to create this object.
	/// </summary>
	class ENGINE_API Entity {
	public:
		virtual ~Entity() = default;
		Entity(const Entity& other) = default;
		Entity(Entity&& other) = default;
		Entity& operator=(const Entity& other) = default;
		Entity& operator=(Entity&& other) = default;

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get the first component of the given type.
		/// </summary>
		/// <returns>Returns the first component of the given type as a weak pointer.</returns>
		std::shared_ptr<ComponentType> GetComponent();

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get all of the components of the given type.
		/// </summary>
		/// <returns>Returns the all of the components of the given type as a weak pointer.</returns>
		std::vector<std::shared_ptr<ComponentType>> GetComponents();

		/// <summary>
		/// Returns all the components of this entity.
		/// </summary>
		/// <returns>Returns a vector of all the components of this entity.</returns>
		std::vector<std::shared_ptr<Component>> GetAllComponents() const;

		template <class ComponentType, class... Args>
		/// <summary>
		/// Allows you to create a component of the given type.
		/// </summary>
		/// <param name="args">The arguments required to initialize the component.</param>
		/// <returns>Returns the just created component of the given type as a weak pointer.</returns>
		std::shared_ptr<ComponentType> AddComponent(Args&&... args);

		//template <class ComponentType, class... Args, std::enable_if_t<std::extent<ComponentType>::value != 0, int> = 0>
		//std::weak_ptr<ComponentType> AddComponent(Args&&... args) = delete;

		template <class ComponentType, class... Args>
		/// <summary>
		/// Allows you to create X amount of components of the given type.
		/// </summary>
		/// <param name="count">The amount of components you want to add of this given type.</param>
		/// <param name="args">The arguments required to initialize the components.</param>
		/// <returns>Returns a vector of the just created components of the given type as a vector of weak pointers.</returns>
		std::vector<std::shared_ptr<ComponentType>> AddComponents(size_t count, Args&&... args);

		template <typename ComponentType>
		/// <summary>
		/// Allows X amount of components of the given type.
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
		virtual void Update();

		/// <summary>
		/// Returns the amount of components on this entity.
		/// </summary>
		/// <returns></returns>
		size_t ComponentCount() const;

		/// <summary>
		/// Returns the currently assigned ID of this entity.
		/// </summary>
		/// <returns></returns>
		int GetId() const;

		/// <summary>
		/// The name of this entity.
		/// </summary>
		std::string name{};

		/// <summary>
		/// This method will return the active state of this entity.
		/// </summary>
		/// <returns>Returns true if this entity is active.</returns>
		bool GetIsActive() const;

		/// <summary>
		/// This method will allow you to change the active state of this entity.
		/// </summary>
		/// <param name="newIsActive">This value will define if this entity will be active or not.</param>
		void SetIsActive(bool newIsActive);

		Sharp::Event<std::shared_ptr<Entity>, std::shared_ptr<Component>> OnComponentAddedEvent;
		Sharp::Event<std::shared_ptr<Entity>, std::shared_ptr<Component>> OnComponentRemovedEvent;

	protected:
		int id{};
		friend class EntitySystem;
		friend class Map;
		std::vector<std::shared_ptr<Component>> components{};

		explicit Entity(std::string name = "");

	private:

		bool isActive{};

		void SetId(int newId);
		
		std::weak_ptr<Entity> GetPointer() const;

		void OnComponentAdded(std::shared_ptr<Component> addedComponent) const;
		void OnComponentRemoved(std::shared_ptr<Component> removedComponent) const;

		// Allows use of OnBeginContact and OnEndContact by EntityContact Callback class
		friend class EntityContact;
		void OnBeginContact(std::shared_ptr<Entity> entity) const;
		void OnEndContact(std::shared_ptr<Entity> entity) const;
	};

	template <typename ComponentType>
	std::shared_ptr<ComponentType> Entity::GetComponent() {
		for (size_t i = 0, size = components.size(); i < size; ++i) {
			if (dynamic_cast<ComponentType*>(components[i].get()))
				return std::dynamic_pointer_cast<ComponentType>(components[i]);
		}
		return nullptr;
	}

	template <typename ComponentType>
	std::vector<std::shared_ptr<ComponentType>> Entity::GetComponents() {
		std::vector<std::shared_ptr<ComponentType>> componentsToReturn;

		for (size_t i = 0, size = components.size(); i < size; ++i) {
			if (dynamic_cast<ComponentType*>(components[i].get()))
				componentsToReturn.push_back(std::dynamic_pointer_cast<ComponentType>(components[i]));
		}

		return componentsToReturn;
	}

	template <class ComponentType, class... Args>
	std::shared_ptr<ComponentType> Entity::AddComponent(Args&&... args) {
		std::shared_ptr<ComponentType> componentToAdd = std::shared_ptr<ComponentType>(new ComponentType(std::forward<Args>(args)...));
		components.push_back(std::move(componentToAdd));

		std::shared_ptr<Component> componentToReturn = components.back();
		componentToReturn->SetOwner(GetPointer().lock());
		OnComponentAdded(componentToReturn);

		for (size_t j = 0, size = components.size(); j < size; ++j)
			componentToReturn->OnComponentAdded(components[j]);

		componentToReturn->InitializeComponent(components);

		std::shared_ptr<Entity> entityPointer = GetPointer().lock();
		OnComponentAddedEvent(entityPointer, componentToReturn);
		return std::static_pointer_cast<ComponentType>(componentToReturn);
	}

	template <class ComponentType, class... Args>
	std::vector<std::shared_ptr<ComponentType>> Entity::AddComponents(const size_t count, Args&&... args) {
		std::vector<std::shared_ptr<ComponentType>> componentsToReturn;

		for (size_t i = 0; i < count; ++i) {
			std::shared_ptr<ComponentType> componentToAdd = std::make_shared<ComponentType>(std::forward<Args>(args)...);
			components.push_back(std::move(componentToAdd));

			std::shared_ptr<Component> componentToReturn = components.back();
			componentToReturn->SetOwner(GetPointer().lock());
			OnComponentAdded(componentToReturn);

			for (size_t j = 0, size = components.size(); j < size; ++j)
				componentToReturn->OnComponentAdded(components[j]);

			componentToReturn->InitializeComponent(components);

			componentsToReturn.push_back(std::static_pointer_cast<ComponentType>(componentToReturn));
			std::shared_ptr<Entity> entityPointer = GetPointer().lock();
			OnComponentAddedEvent(entityPointer, componentToReturn);
		}
		return componentsToReturn;
	}

	template <typename ComponentType>
	void Entity::RemoveComponent(size_t amountToRemove) {
		if (amountToRemove != 1 && amountToRemove > components.size())
			amountToRemove = components.size();

		for (size_t i = components.size() - 1; i != -1; --i) {
			if (dynamic_cast<ComponentType*>(components[i].get())) {
				std::shared_ptr<Component> componentToRemove = components[i];
				components.erase(components.begin() + i);
				OnComponentRemoved(componentToRemove);
				std::shared_ptr<Entity> entityPointer = GetPointer().lock();
				OnComponentAddedEvent(entityPointer, componentToRemove);
			}
			amountToRemove--;
			if (amountToRemove == 0)
				break;
		}
	}

	template <typename ComponentType>
	void Entity::RemoveAllComponents() {
		for (size_t i = components.size() - 1; i != -1; --i) {
			if (dynamic_cast<ComponentType*>(components[i].get())) {
				std::shared_ptr<Component> componentToRemove = components[i];
				components.erase(components.begin() + i);
				OnComponentRemoved(componentToRemove);
				std::shared_ptr<Entity> entityPointer = GetPointer().lock();
				OnComponentAddedEvent(entityPointer, componentToRemove);
			}
		}
	}
} // namespace Engine
