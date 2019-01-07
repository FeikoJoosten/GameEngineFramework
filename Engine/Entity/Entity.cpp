#include "Engine/Entity/Entity.hpp"
#include "Engine/engine.hpp"
#include "Engine/Entity/EntitySystem.hpp"

namespace Engine
{
	Entity::Entity(eastl::string name) : name(name), Id(-1), isActive(true)
	{
	}

	void Entity::InitializeEntity()
	{
	}

	eastl::vector<eastl::shared_ptr<Component>> Entity::GetAllComponents() const
	{
		return components;
	}

	eastl::shared_ptr<Entity> Entity::GetPointer() const
	{
		return Engine::GetEngine().lock()->GetEntitySystem().lock()->GetEntity(Id).lock();
	}

	void Entity::Update()
	{
		if (isActive == false)
			return;

		for (size_t i = 0, size = components.size(); i < size; ++i)
		{
			if (components[i]->isEnabled == false)
				continue;

			components[i]->Update();
		}
	}

	size_t Entity::ComponentCount() const
	{
		return components.size();
	}

	uint64_t Entity::GetID() const
	{
		return Id;
	}

	bool Entity::GetIsActive() const
	{
		return isActive;
	}

	void Entity::SetIsActive(bool isActive)
	{
		this->isActive = isActive;
	}

	void Entity::SetID(uint64_t Id)
	{
		this->Id = Id;
	}

	void Entity::OnComponentAdded(eastl::weak_ptr<Component> addedComponent)
	{
		// No need to inform the last added component that it just has been added.
		for (size_t i = 0, size = components.size() - 1; i < size; ++i)
			components[i]->OnComponentAdded(addedComponent);
	}

	void Entity::OnComponentRemoved(eastl::weak_ptr<Component> removedComponent)
	{
		for (size_t i = 0, size = components.size(); i < size; ++i)
			components[i]->OnComponentRemoved(removedComponent);
	}

	void Entity::OnBeginContact(eastl::weak_ptr<Entity> entity)
	{
		for (size_t i = 0, size = components.size(); i < size; ++i)
			components[i]->OnBeginContact(entity);
	}

	void Entity::OnEndContact(eastl::weak_ptr<Entity> entity)
	{
		for (size_t i = 0, size = components.size(); i < size; ++i)
			components[i]->OnEndContact(entity);
	}
} // namespace Engine