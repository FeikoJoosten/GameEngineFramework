#pragma once

#include "Engine/Api.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Components/ModelComponent.hpp"

namespace Engine {

	class ENGINE_API AnimationComponent : public Component
	{
	public:
		virtual ~AnimationComponent() override = default;
		AnimationComponent(const AnimationComponent& other) = delete;
		AnimationComponent(AnimationComponent&& other) noexcept = delete;

		AnimationComponent& operator=(const AnimationComponent& other) = delete;
		AnimationComponent& operator=(AnimationComponent&& other) noexcept = delete;
		
	private:
		friend class Entity;

		explicit AnimationComponent() noexcept = default;

		virtual void Update() override;

		virtual void OnComponentAdded(std::shared_ptr<Component> addedComponent) override;

		virtual void OnComponentRemoved(std::shared_ptr<Component> removedComponent) override;
	};

}
