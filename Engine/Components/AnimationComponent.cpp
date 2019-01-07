#include "Engine/Components/AnimationComponent.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/engine.hpp"

namespace Engine {
	
	AnimationComponent::~AnimationComponent()
	{
	}

	eastl::vector<eastl::weak_ptr<ModelComponent>> AnimationComponent::GetEntityModels() const
	{
		return entityModels;
	}

	eastl::vector<eastl::string> AnimationComponent::GetModelAnimations(eastl::weak_ptr<ModelComponent> modelComponent)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				return GetModelAnimations(i);
			}
		}
		return eastl::vector<eastl::string>();
	}

	eastl::vector<eastl::string> AnimationComponent::GetModelAnimations(size_t index)
	{
		if (entityModels.size()>index) {
			if (!entityModels[index].expired())
				return entityModels[index].lock()->GetModel().lock()->GetAnimations();
		}
		return eastl::vector<eastl::string>();
	}

	void AnimationComponent::SetModelAnimation(eastl::weak_ptr<ModelComponent> modelComponent, eastl::string name, bool resetTime)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				SetModelAnimation(i, name, resetTime);
			}
		}
	}

	void AnimationComponent::SetModelAnimation(size_t index, eastl::string name, bool resetTime)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				entityModels[index].lock()->GetModel().lock()->SetAnimation(name, resetTime);
	}

	eastl::string AnimationComponent::GetModelCurrentAnimation(eastl::weak_ptr<ModelComponent> modelComponent)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				GetModelCurrentAnimation(i);
			}
		}
		return "";
	}

	eastl::string AnimationComponent::GetModelCurrentAnimation(size_t index)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				return entityModels[index].lock()->GetModel().lock()->GetCurrentAnimationName();
		return "";
	}

	void AnimationComponent::ResetModelAnimation(eastl::weak_ptr<ModelComponent> modelComponent)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				ResetModelAnimation(i);
			}
		}
	}

	void AnimationComponent::ResetModelAnimation(size_t index)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				entityModels[index].lock()->GetModel().lock()->ResetAnimation();
	}

	void AnimationComponent::SetModelAnimationTime(eastl::weak_ptr<ModelComponent> modelComponent, float time)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				SetModelAnimationTime(i, time);
			}
		}
	}

	void AnimationComponent::SetModelAnimationTime(size_t index, float time)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				entityModels[index].lock()->GetModel().lock()->SetAnimationTime(time);
	}

	float AnimationComponent::GetModelAnimationTime(eastl::weak_ptr<ModelComponent> modelComponent)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				return GetModelAnimationTime(i);
			}
		}
		return 0.0f;
	}

	float AnimationComponent::GetModelAnimationTime(size_t index)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				return entityModels[index].lock()->GetModel().lock()->GetAnimationTime();
		return 0.0f;
	}

	float AnimationComponent::GetModelAnimationDuration(eastl::weak_ptr<ModelComponent> modelComponent)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				return GetModelAnimationDuration(i);
			}
		}
		return 0.0f;
	}

	float AnimationComponent::GetModelAnimationDuration(size_t index)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				return entityModels[index].lock()->GetModel().lock()->GetAnimationDuration();
		return 0.0f;
	}

	void AnimationComponent::SetModelAnimationPaused(eastl::weak_ptr<ModelComponent> modelComponent, bool paused)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				SetModelAnimationPaused(i, paused);
			}
		}
	}

	void AnimationComponent::SetModelAnimationPaused(size_t index, bool paused)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				entityModels[index].lock()->GetModel().lock()->SetPaused(paused);
	}

	bool AnimationComponent::isModelAnimationPaused(eastl::weak_ptr<ModelComponent> modelComponent)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				return isModelAnimationPaused(i);
			}
		}
		return false;
	}

	bool AnimationComponent::isModelAnimationPaused(size_t index)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				return entityModels[index].lock()->GetModel().lock()->IsPaused();
		return true;
	}

	void AnimationComponent::SetModelAnimationLooping(eastl::weak_ptr<ModelComponent> modelComponent, bool looping)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				SetModelAnimationLooping(i, looping);
			}
		}
	}

	void AnimationComponent::SetModelAnimationLooping(size_t index, bool looping)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				entityModels[index].lock()->GetModel().lock()->SetLooping(looping);
	}

	bool AnimationComponent::IsModelAnimationLooping(eastl::weak_ptr<ModelComponent> modelComponent)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				return IsModelAnimationLooping(i);
			}
		}
		return false;
	}

	bool AnimationComponent::IsModelAnimationLooping(size_t index)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				return entityModels[index].lock()->GetModel().lock()->IsLooping();
		return false;
	}

	void AnimationComponent::SetModelAnimationSpeed(eastl::weak_ptr<ModelComponent> modelComponent, float speed)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				SetModelAnimationSpeed(i, speed);
			}
		}
	}

	void AnimationComponent::SetModelAnimationSpeed(size_t index, float speed)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				entityModels[index].lock()->GetModel().lock()->SetSpeed(speed);
	}

	float AnimationComponent::GetModelAnimationSpeed(eastl::weak_ptr<ModelComponent> modelComponent)
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].lock() == modelComponent.lock()) {
				return GetModelAnimationSpeed(i);
			}
		}
		return 0.0f;
	}

	float AnimationComponent::GetModelAnimationSpeed(size_t index)
	{
		if (index < entityModels.size())
			if (!entityModels[index].expired())
				return entityModels[index].lock()->GetModel().lock()->GetSpeed();
		return 0.0f;
	}

	AnimationComponent::AnimationComponent() noexcept
	{
	}

	void AnimationComponent::InitializeComponent()
	{
		isEnabled = true;
	}

	void AnimationComponent::Update()
	{
		for (size_t i = 0, size = entityModels.size(); i < size; ++i) {
			if (entityModels[i].expired())
				continue;

			if (!entityModels[i].lock()->isEnabled)
				continue;

			entityModels[i].lock()->GetModel().lock()->UpdateAnimation(
				Engine::GetEngine().lock()->GetTime().lock()->GetDeltaTime());
		}
	}

	void AnimationComponent::OnComponentAdded(eastl::weak_ptr<Component> addedComponent)
	{
		if (eastl::dynamic_pointer_cast<ModelComponent, Component>(addedComponent.lock())) {
			if (eastl::dynamic_pointer_cast<ModelComponent, Component>(addedComponent.lock())->GetModel().lock()->HasAnimations())
				entityModels.push_back(eastl::dynamic_pointer_cast<ModelComponent, Component>(addedComponent.lock()));
		}
	}

	void AnimationComponent::OnComponentRemoved(eastl::weak_ptr<Component> removedComponent)
	{
		if (eastl::dynamic_pointer_cast<ModelComponent, Component>(removedComponent.lock())) {
			eastl::vector<eastl::weak_ptr<ModelComponent>>::iterator it;
			for (it = entityModels.begin(); it != entityModels.end(); ++it) {
				if (it->lock() == removedComponent.lock())
					break;
			}
			if (it != entityModels.end()) {
				entityModels.erase(it);
			}
		}
	}

}