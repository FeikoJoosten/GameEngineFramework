#include <utility>

#include "Engine/Components/Component.hpp"
#include "Engine/Entity/Entity.hpp"

namespace Engine {
	Component::~Component() {
		if (!owner.expired())
			owner.lock()->OnActiveStateChangedEvent -= Sharp::EventHandler::Bind(this, &Component::HandleOnOwnerActiveStateChangedEvent);
	}

	std::shared_ptr<Entity> Component::GetOwner() const {
		return owner.lock();
	}

	bool Component::GetIsEnabled() const {
		return isEnabled;
	}

	void Component::SetIsEnabled(const bool newIsEnabled) {
		isEnabled = newIsEnabled;
	}

	bool Component::GetIsActiveAndEnabled() const {
		return isEnabled && (owner.expired() || owner.lock()->GetIsActive());
	}

	void Component::Update() {}

	void Component::OnComponentAdded(const std::shared_ptr<Component> addedComponent) {}

	void Component::OnComponentRemoved(const std::shared_ptr<Component> removedComponent) {}

	std::vector<std::shared_ptr<Component>> Component::GetAllComponents() const {
		if (owner.expired()) return {};
		return owner.lock()->GetAllComponents();
	}

	void Component::SetOwner(const std::shared_ptr<Entity> newOwner) {
		if (!owner.expired())
			owner.lock()->OnActiveStateChangedEvent -= Sharp::EventHandler::Bind(this, &Component::HandleOnOwnerActiveStateChangedEvent);

		owner = newOwner;

		if (!owner.expired())
			owner.lock()->OnActiveStateChangedEvent += Sharp::EventHandler::Bind(this, &Component::HandleOnOwnerActiveStateChangedEvent);
	}

	void Component::HandleOnOwnerActiveStateChangedEvent(const std::shared_ptr<Entity> owningEntity, const bool owningEntityIsActive) {
		OnEnabledStateChangedEvent(shared_from_this(), owningEntityIsActive && isEnabled);
	}
} // namespace Engine
