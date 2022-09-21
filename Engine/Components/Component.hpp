#pragma once

#include "Engine/Api.hpp"
#include <vector>
#include <cereal/cereal.hpp>

#include "Engine/Utility/Event.hpp"

namespace Engine {
	class Entity;

	/// <summary>
	/// This is the base class for components. NOTE: only the Entity class is allowed to create this object.
	/// </summary>
	class ENGINE_API Component {
	public:
		friend class Entity;
		friend class cereal::access;

		Sharp::Event<std::shared_ptr<Component>, bool> OnEnabledStateChangedEvent;

		Component() = default;
		virtual ~Component();
		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		/// <summary>
		/// This method allows you to get the entity holding this component.
		/// </summary>
		/// <returns>Returns the owner of this component.</returns>
		std::weak_ptr<Entity> GetOwner() const;

		/// <summary></summary>
		/// <returns>Returns the enabled state of this component.</returns> 
		[[nodiscard]] bool GetIsEnabled() const;

		/// <summary>Allows you to set the enabled state of a component.</summary>
		///	<param name="newIsEnabled">The new enabled state of this component.</param>
		void SetIsEnabled(bool newIsEnabled);

		/// <summary>Allows you to get the enabled state of the component and the active state of the owning entity</summary>
		/// <returns>Returns true if the component and the owner are enabled and active.</returns> 
		[[nodiscard]] bool GetIsActiveAndEnabled() const;

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
		/// <returns>Returns the just created component of the given type as a shared pointer.</returns>
		std::shared_ptr<ComponentType> AddComponent(Args&&... args);

		template <class ComponentType, class... Args>
		/// <summary>
		/// Allows you to create X amount of components of the given type.
		/// </summary>
		/// <param name="count">The amount of components you want to add of this given type.</param>
		/// <param name="args">The arguments required to initialize the components.</param>
		/// <returns>Returns a vector of the just created components of the given type as a vector of shared pointers.</returns>
		std::vector<std::shared_ptr<ComponentType>> AddComponents(size_t count, Args&&... args);

		template <typename ComponentType>
		/// <summary>
		/// Allows X amount of components of the given type.
		/// </summary>
		/// <param name="amountToRemove">Will remove 1 component by default, can be change optionally.</param>T
		void RemoveComponent(size_t amountToRemove = 1) const;

		template <typename ComponentType>
		/// <summary>
		/// Deletes all the components of the given type.
		/// </summary>
		void RemoveAllComponents() const;

	protected:

		/// <summary>
		/// This method allows you to get a direct reference to the weak pointer of this component.
		/// </summary>
		/// <returns>Returns the reference of this object as a weak pointer.</returns>
		[[nodiscard]] std::shared_ptr<Component> GetPointerReference() const;

		/// <summary>
		/// This method allows you to get a direct reference to the weak pointer of this component.
		/// </summary>
		/// <returns>Returns the reference of this object as a weak pointer.</returns>
		template<typename ComponentType>
		[[nodiscard]] std::shared_ptr<ComponentType> GetPointerReference() const;

		/// <summary>
		/// This method is used to initialize components in. This method is called after the setting of the owner.
		/// </summary>
		///	<param name="availableComponents">All components currently added onto the owning Entity. This also includes the just added component </param>
		virtual void InitializeComponent(const std::vector<std::shared_ptr<Component>>& availableComponents);

		/// <summary>
		/// The update method of this component. NOTE: This method will not be called in case the entity is disabled.
		/// </summary>
		virtual void Update();

		/// <summary>
		/// This method will be called whenever a component has been added to the entity.
		/// </summary>
		/// <param name="addedComponent">The component that has been added.</param>
		virtual void OnComponentAdded(std::shared_ptr<Component> addedComponent);

		/// <summary>
		/// This method will be called whenever a component has been removed from the entity.
		/// </summary>
		/// <param name="removedComponent">The component that has been removed.</param>
		virtual void OnComponentRemoved(std::shared_ptr<Component> removedComponent);

	private:
		bool isEnabled = true;
		std::weak_ptr<Entity> owner;
		std::weak_ptr<Component> pointerReference;

		void SetOwner(std::shared_ptr<Entity> newOwner);

		void SetPointerReference(std::weak_ptr<Component> newPointerReference);

		void HandleOnOwnerActiveStateChangedEvent(std::shared_ptr<Entity> owningEntity, bool owningEntityIsActive);

		template<class Archive>
		void Serialize(Archive& archive);
	};

	template <typename ComponentType>
	std::shared_ptr<ComponentType> Component::GetComponent() {
		if (owner.expired()) return {};
		return owner.lock()->GetComponent<ComponentType>();
	}

	template <typename ComponentType>
	std::vector<std::shared_ptr<ComponentType>> Component::GetComponents() {
		if (owner.expired()) return {};
		return owner.lock()->GetComponents<ComponentType>();
	}

	template <class ComponentType, class ... Args>
	std::shared_ptr<ComponentType> Component::AddComponent(Args&&... args) {
		if (owner.expired()) return {};
		return owner.lock()->AddComponent<ComponentType>(&args...);
	}

	template <class ComponentType, class ... Args>
	std::vector<std::shared_ptr<ComponentType>> Component::AddComponents(size_t count, Args&&... args) {
		if (owner.expired()) return {};
		return owner.lock()->AddComponents<ComponentType>(count, &args...);
	}

	template <typename ComponentType>
	void Component::RemoveComponent(size_t amountToRemove) const {
		if (owner.expired()) return {};
		owner.lock()->RemoveComponent<ComponentType>(amountToRemove);
	}

	template <typename ComponentType>
	void Component::RemoveAllComponents() const {
		if (owner.expired()) return {};
		owner.lock()->RemoveAllComponents<ComponentType>();
	}
} // namespace Engine