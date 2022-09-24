#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Components/Component.hpp"
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
		template <class ComponentType, class... Args>
		friend std::shared_ptr<ComponentType> Entity::AddComponent(Args&&... args);

		/// <summary>
		/// Create a model component with the give Model.
		/// </summary>
		explicit ModelComponent(std::shared_ptr<Model> model) noexcept;

		/// <summary>
		/// Create a model component with the given path.
		/// </summary>
		/// <param name="path">This path value is used to load the default mesh for this model component.</param>
		/// <returns></returns>
		explicit ModelComponent(const std::string& path) noexcept;

	public:
		ModelComponent() = delete;

	private:
		std::weak_ptr<Time> time;

		virtual void Update() override;
	};
} //namespace Engine
