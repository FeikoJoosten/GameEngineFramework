#pragma once

#include "Engine/api.hpp"

#include <ThirdParty/cereal/include/cereal/cereal.hpp>
#include <ThirdParty/EASTL-master/include/EASTL/shared_ptr.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	class Entity;

	/// <summary>
	/// This is the base class for components. NOTE: only the Entity class is allowed to create this object.
	/// </summary>
	class ENGINE_API Component
	{
	public:
		virtual ~Component() = default;
		/// <summary>
		/// This method allows you to get the entity holding this component.
		/// </summary>
		/// <returns>Returns the owner of this component.</returns>
		eastl::weak_ptr<Entity> GetOwner() const;

		bool isEnabled;

	protected:
		friend class Entity;
		explicit Component();

		/// <summary>
		/// This method is used to initialize your component in. This method is called after the setting of the owner. 
		/// </summary>
		virtual void InitializeComponent();

		/// <summary>
		/// The update method of this component. NOTE: This method will not be called in case the entity is disabled.
		/// </summary>
		virtual void Update();

		/// <summary>
		/// This method will be called whenever a component has been added to the entity.
		/// </summary>
		/// <param name="addedComponent">The component that has been added.</param>
		virtual void OnComponentAdded(eastl::weak_ptr<Component> addedComponent);

		/// <summary>
		/// This method will be called whenever a component has been removed from the entity.
		/// </summary>
		/// <param name="removedComponent">The component that has been removed.</param>
		virtual void OnComponentRemoved(eastl::weak_ptr<Component> removedComponent);

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
		void RemoveComponent(size_t amountToRemove = 1) const;

		template <typename ComponentType>
		/// <summary>
		/// Deletes all the components of the given type.
		/// </summary>
		void RemoveAllComponents() const;

		/// <summary>
		/// Saves the data of this component to a archive (all derived classes have their own version, but if needed they can call this one as a failsafe/default version)
		/// </summary>
		/// <param name="ar"> the archive that you want to save to </param>
		template <typename archive>
		void SaveBaseComponent(archive ar);

		/// <summary>
		/// loads the data from the archive to this component (all derived classes have their own version, but if needed they can call this one as a failsafe/default version)
		/// </summary>
		/// <param name="ar"> the archive that you want to load from </param>
		template <typename archive>
		void LoadBaseComponent(archive ar);

		virtual void OnBeginContact(eastl::weak_ptr<Entity> entity);
		virtual void OnEndContact(eastl::weak_ptr<Entity> entity);

		/// <summary>
		/// This method allows you to get a direct refence to the weak pointer of this component.
		/// </summary>
		/// <returns>Returns the reference of this object as a weak pointer.</returns>
		eastl::weak_ptr<Component> GetPointerRefence() const;

		template <typename ComponentType>
		/// <summary>
		/// This method allows you to get a direct refence to the weak pointer of this component.
		/// </summary>
		/// <returns>Returns the reference of this object as a weak pointer.</returns>
		eastl::weak_ptr<ComponentType> GetPointerRefence();
	private:
		/// <summary>
		/// Sets the owner of this components. Only callable by the Entity class.
		/// </summary>
		/// <param name="owner">The new owner of this class.</param>
		void SetOwner(eastl::weak_ptr<Entity> owner);

		void SetPointerReference(eastl::weak_ptr<Component> pointerReference);

		eastl::weak_ptr<Entity> owner;
		eastl::weak_ptr<Component> pointerReference;

	};

	template <typename ComponentType>
	eastl::weak_ptr<ComponentType> Component::GetComponent()
	{
		return owner.lock()->GetComponent<ComponentType>();
	}

	template <typename ComponentType>
	eastl::vector<eastl::weak_ptr<ComponentType>> Component::GetComponents()
	{
		return owner.lock()->GetComponents<ComponentType>();
	}

	template <class ComponentType, class ... Args>
	eastl::weak_ptr<ComponentType> Component::AddComponent(Args&&... args)
	{
		return owner.lock()->AddComponent<ComponentType>(&args...);
	}

	template <class ComponentType, class ... Args>
	eastl::vector<eastl::weak_ptr<ComponentType>> Component::AddComponents(size_t count, Args&&... args)
	{
		return owner.lock()->AddComponents<ComponentType>(count, &args...);
	}

	template <typename ComponentType>
	void Component::RemoveComponent(size_t amountToRemove) const
	{
		owner.lock()->RemoveComponent<ComponentType>(amountToRemove);
	}

	template <typename ComponentType>
	void Component::RemoveAllComponents() const
	{
		owner.lock()->RemoveAllComponents<ComponentType>();
	}

	template <typename ComponentType>
	eastl::weak_ptr<ComponentType> Component::GetPointerRefence()
	{
		return eastl::dynamic_pointer_cast<ComponentType, Component>(pointerReference.lock());
	}
} // namespace Engine