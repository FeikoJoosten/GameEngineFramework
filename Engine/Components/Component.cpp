#include "Engine/Components/Component.hpp"
#include "Engine/Entity/Entity.hpp"

namespace Engine {
	std::weak_ptr<Entity> Component::GetOwner() const {
		return owner;
	}

	bool Component::GetIsEnabled() const {
		return isEnabled;
	}

	void Component::SetIsEnabled(const bool newIsEnabled) {
		isEnabled = newIsEnabled;
	}

	void Component::InitializeComponent(const std::vector<std::shared_ptr<Component>>& availableComponents) {}

	void Component::Update() {}

	void Component::OnComponentAdded(const std::shared_ptr<Component> addedComponent) {}

	void Component::OnComponentRemoved(const std::shared_ptr<Component> removedComponent) {}

	std::vector<std::shared_ptr<Component>> Component::GetAllComponents() const {
		if (owner.expired()) return {};
		return owner.lock()->GetAllComponents();
	}

	void Component::OnBeginContact(const std::shared_ptr<Entity> entity) {}

	void Component::OnEndContact(const std::shared_ptr<Entity> entity) {}

	void Component::SetOwner(const std::shared_ptr<Entity> newOwner) {
		owner = newOwner;
	}

	template<class Archive>
	void Component::Serialize(Archive& archive) {
		archive(CEREAL_NVP(isEnabled));
	}
} // namespace Engine
