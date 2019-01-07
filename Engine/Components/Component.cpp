#include "Engine/Components/Component.hpp"
#include "Engine/Entity/Entity.hpp"

#include <ThirdParty/cereal/include/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/include/cereal/archives/json.hpp>

namespace Engine
{
	eastl::weak_ptr<Entity> Component::GetOwner() const
	{
		return owner;
	}

	Component::Component() : isEnabled(true)
	{
	}

	void Component::InitializeComponent()
	{
	}

	void Component::Update()
	{
	}

	void Component::OnComponentAdded(eastl::weak_ptr<Component> addedComponent)
	{
	}

	void Component::OnComponentRemoved(eastl::weak_ptr<Component> removedComponent)
	{
	}

	eastl::vector<eastl::shared_ptr<Component>> Component::GetAllComponents() const
	{
		return owner.lock()->GetAllComponents();
	}

	void Component::OnBeginContact(eastl::weak_ptr<Entity> entity)
	{
	}

	void Component::OnEndContact(eastl::weak_ptr<Entity> entity)
	{
	}

	eastl::weak_ptr<Component> Component::GetPointerRefence() const
	{
		return pointerReference;
	}

	void Component::SetOwner(eastl::weak_ptr<Entity> owner)
	{
		this->owner = owner;
	}

	void Component::SetPointerReference(eastl::weak_ptr<Component> pointerReference)
	{
		this->pointerReference = pointerReference;
	}

	template <typename archive>
	void Component::SaveBaseComponent(archive ar)
	{
		ar(CEREAL_NVP(this->isEnabled));
	}

	template <typename archive>
	void Component::LoadBaseComponent(archive ar)
	{
		ar(this->isEnabled);
	}
} // namespace Engine
