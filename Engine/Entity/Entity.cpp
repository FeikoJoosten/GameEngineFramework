#include "Engine/Entity/Entity.hpp"
#include "Engine/engine.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Components/ModelComponent.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include <iostream>

namespace Engine
{
	eastl::vector<eastl::shared_ptr<Component>> Entity::GetAllComponents()
	{
		return components;
	}

	void Entity::AddComponent(eastl::unique_ptr<Component> &&componentToAdd)
	{
		components.push_back(eastl::move(componentToAdd));
	}

	void Entity::AddComponents(eastl::vector<eastl::unique_ptr<Component>>& componentsToAdd)
	{
		for (size_t i = 0, size = componentsToAdd.size(); i < size; ++i)
			components.push_back(eastl::move(componentsToAdd[i]));
	}

	void Entity::RemoveComponent(Component componentToRemove)
	{
		for (auto it = components.begin(); it != components.end(); it++)
		{
			if (typeid(it) == typeid(&componentToRemove))
			{
				components.erase(it);
				break;
			}
		}

		components.shrink_to_fit();
	}

	void Entity::RemoveComponents(eastl::vector<Component>& componentsToRemove)
	{
		for (auto it = components.begin(); it != components.end(); it++)
		{
			for (auto componentsIt = componentsToRemove.begin(), componentsEnd = componentsToRemove.end(); componentsIt != componentsEnd; ++componentsIt)
			{
				std::cout << typeid(it).name() << "      " << typeid(componentsIt).name() << std::endl;
				if (typeid(it) == typeid(componentsIt))
				{
					components.erase(it);
					break;
				}
			}
		}

		components.shrink_to_fit();
	}

	void Entity::Update()
	{
		for (auto it = components.begin(), end = components.end(); it != end; ++it)
		{
			
		}
	}
	
	void Entity::Render()
	{
		auto transform = GetComponent<TransformComponent>();
		if (transform == nullptr) return;

		auto model = GetComponent<ModelComponent>();
		if (model == nullptr) return;

		Engine::GetRenderer().Render(transform->GetModelMatrix(), model->GetModel());
	}

	size_t Entity::ComponentCount()
	{
		return components.size();
	}

	int Entity::GetTeamID()
	{
		return teamID;
	}

	int Entity::GetID()
	{
		return int(ID);
	}

	void Entity::SetTeamID(int teamID)
	{
		this->teamID = teamID;
	}

	void Entity::SetID(glm::uint_t ID)
	{
		this->ID = ID;
	}
} // namespace Engine