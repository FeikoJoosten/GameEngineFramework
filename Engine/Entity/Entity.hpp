#pragma once
#include "Engine/api.hpp"
#include "Engine/Utility/Logging.hpp"
#include "Engine/Components/Component.hpp"

#include <ThirdParty/EASTL-master/include/EASTL/string.h>

namespace Engine
{
	/// <summary>
	/// This object is able a holder object for components. NOTE: only the EntitySystem is allowed to create this object.
	/// </summary>
	class ENGINE_API Entity
	{
	public:

		virtual ~Entity() = default;

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get the first component of the given type.
		/// </summary>
		/// <returns>Returns the first component of the given type as a weak pointer.</returns>
		eastl::weak_ptr<ComponentType> GetComponent();

		template<typename ComponentType>
		/// <summary>
		/// This method allows you to get all of the components of the given type.
		/// </summary>
		/// <returns>Returns the all of the components of the given type as a weak pointer.</returns>
		eastl::vector<eastl::weak_ptr<ComponentType>> GetComponents();

		/// <summary>
		/// Returns all the components of this entity.
		/// </summary>
		/// <returns>Returns a vector of all the components of this entity.</returns>
		eastl::vector<eastl::shared_ptr<Component>> GetAllComponents() const;

		template <class ComponentType, class... Args>
		/// <summary>
		/// Allows you to create a component of the given type.
		/// </summary>
		/// <param name="...args">The arguments required to initialize the component.</param>
		/// <returns>Returns the just created component of the given type as a weak pointer.</returns>
		eastl::weak_ptr<ComponentType> AddComponent(Args&&... args);

		//template <class ComponentType, class... Args, eastl::enable_if_t<eastl::extent<ComponentType>::value != 0, int> = 0>
		//eastl::weak_ptr<ComponentType> AddComponent(Args&&... args) = delete;

		template <class ComponentType, class... Args>
		/// <summary>
		/// Allows you to create X amount of components of the given type.
		/// </summary>
		/// <param name="count">The amount of components you want to add of this given type.</param>
		/// <param name="...args">The arguments required to initialize the components.</param>
		/// <returns>Returns a vector of the just created components of the given type as a vector of weak pointers.</returns>
		eastl::vector<eastl::weak_ptr<ComponentType>> AddComponents(size_t count, Args&&... args);

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
		/// Allows you to retrieve current shared pointer from the entity system.
		/// </summary>
		/// <returns></returns>
		eastl::shared_ptr<Entity> GetPointer() const;

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
		uint64_t GetID() const;

		/// <summary>
		/// The name of this entity.
		/// </summary>
		eastl::string name;

		/// <summary>
		/// This method will return the active state of this entity.
		/// </summary>
		/// <returns>Returns true if this entity is active.</returns>
		bool GetIsActive() const;

		/// <summary>
		/// This method will allow you to change the active state of this entity.
		/// </summary>
		/// <param name="isActive">This value will define if this entity will be active or not.</param>
		void SetIsActive(bool isActive);

	protected:
		uint64_t Id;
		friend class EntitySystem;
		friend class Map;
		eastl::vector<eastl::shared_ptr<Component>> components;
		explicit Entity(eastl::string name = "");
		virtual void InitializeEntity();

	private:

		bool isActive;
		void SetID(uint64_t Id);
		void OnComponentAdded(eastl::weak_ptr<Component> addedComponent);
		void OnComponentRemoved(eastl::weak_ptr<Component> removedComponent);

		// Allows use of OnBeginContact and OnEndContact by EntityContact Callback class
		friend class EntityContact;
		void OnBeginContact(eastl::weak_ptr<Entity> entity);
		void OnEndContact(eastl::weak_ptr<Entity> entity);
	};

	template <typename ComponentType>
	eastl::weak_ptr<ComponentType> Entity::GetComponent()
	{
		for (size_t i = 0, size = components.size(); i < size; ++i)
		{
			if (dynamic_cast<ComponentType*>(components[i].get()))
			{
				return eastl::dynamic_pointer_cast<ComponentType>(components[i]);
			}
		}
		return eastl::weak_ptr<ComponentType>();
	}

	template <typename ComponentType>
	eastl::vector<eastl::weak_ptr<ComponentType>> Entity::GetComponents()
	{
		eastl::vector<eastl::weak_ptr<ComponentType>> componentsToReturn;

		for (size_t i = 0, size = components.size(); i < size; ++i)
		{
			if (dynamic_cast<ComponentType*>(components[i].get()))
				componentsToReturn.push_back(eastl::dynamic_pointer_cast<ComponentType>(components[i]));
		}

		return componentsToReturn;
	}

	template <class ComponentType, class... Args>
	eastl::weak_ptr<ComponentType> Entity::AddComponent(Args&&... args)
	{
		eastl::shared_ptr<ComponentType> componentToAdd = eastl::shared_ptr<ComponentType>(new ComponentType(eastl::forward<Args>(args)...));
		components.push_back(eastl::move(componentToAdd));

		eastl::shared_ptr<Component> componentToReturn = components.back();
		componentToReturn->SetOwner(GetPointer());
		componentToReturn->SetPointerReference(componentToReturn);
		OnComponentAdded(componentToReturn);

		for (size_t j = 0, size = components.size(); j < size; ++j)
			componentToReturn->OnComponentAdded(components[j]);

		componentToReturn->InitializeComponent();

		return eastl::static_pointer_cast<ComponentType>(componentToReturn);
	}

	template <class ComponentType, class... Args>
	eastl::vector<eastl::weak_ptr<ComponentType>> Entity::AddComponents(size_t count, Args&&... args)
	{
		eastl::vector<eastl::weak_ptr<ComponentType>> componentsToReturn;

		for (size_t i = 0; i < count; ++i)
		{
			eastl::shared_ptr<ComponentType> componentToAdd = eastl::shared_ptr<ComponentType>(new ComponentType(eastl::forward<Args>(args)...));
			components.push_back(eastl::move(componentToAdd));

			eastl::shared_ptr<Component> componentToReturn = components.back();
			componentToReturn->SetOwner(GetPointer());
			componentToReturn->SetPointerReference(componentToReturn);
			OnComponentAdded(componentToReturn);

			for (size_t j = 0, size = components.size(); j < size; ++j)
				componentToReturn->OnComponentAdded(components[j]);

			componentToReturn->InitializeComponent();

			componentsToReturn.push_back(eastl::static_pointer_cast<ComponentType>(componentToReturn));
		}
		return componentsToReturn;
	}

	template <typename ComponentType>
	void Entity::RemoveComponent(size_t amountToRemove)
	{
		if (amountToRemove != 1 && amountToRemove > components.size())
			amountToRemove = components.size();

		for (size_t i = components.size() - 1; i != -1; --i)
		{
			if (dynamic_cast<ComponentType*>(components[i].get()))
			{
				eastl::weak_ptr<Component> componentToRemove = components[i];
				components.erase(components.begin() + i);
				OnComponentRemoved(componentToRemove);
			}
			amountToRemove--;
			if (amountToRemove == 0)
				break;
		}
	}

	template <typename ComponentType>
	void Entity::RemoveAllComponents()
	{
		for (size_t i = components.size() - 1; i != -1; --i)
		{
			if (dynamic_cast<ComponentType*>(components[i].get()))
			{
				eastl::weak_ptr<Component> componentToRemove = components[i];
				components.erase(components.begin() + i);
				OnComponentRemoved(componentToRemove);
			}
		}
	}
} // namespace Engine
