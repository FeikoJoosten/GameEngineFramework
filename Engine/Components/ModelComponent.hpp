#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Model/Model.hpp"

namespace Engine
{
	class Time;
	class TransformComponent;

	/// <summary>
	/// This component is used to keep track of the mesh and material combination to render. NOTE: only the Entity class is allowed to create this component.
	/// </summary>
	class ENGINE_API ModelComponent : public RenderComponent
	{
		friend cereal::access;
		template <class ComponentType>
		friend std::shared_ptr<ComponentType> Entity::AddComponent();

		ModelComponent() = default;

		std::weak_ptr<Time> time;

	public:
		virtual ~ModelComponent() override = default;
		ModelComponent(const ModelComponent& other) = delete;
		ModelComponent(ModelComponent&& other) = delete;
		ModelComponent& operator=(const ModelComponent& other) = delete;
		ModelComponent& operator=(ModelComponent&& other) = delete;

	protected:
		virtual void Update() override;

	private:
		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void ModelComponent::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<RenderComponent>(this))
		);
	}
} //namespace Engine
