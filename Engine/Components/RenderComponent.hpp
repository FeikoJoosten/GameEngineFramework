#pragma once

#include "Engine/Api.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Engine {
	class Material;
	class Model;
	class TransformComponent;

	class ENGINE_API RenderComponent : public Component {
		template <class ComponentType, class... Args>
		friend std::shared_ptr<ComponentType> Entity::AddComponent(Args&&... args);
		friend void Renderer::RenderFrame(const glm::mat4x4& view, const glm::mat4x4& projection);

	protected:
		RenderComponent() = default;

		/// <summary>
		/// Create a model component with the give Model.
		/// </summary>
		explicit RenderComponent(std::shared_ptr<Model> model) noexcept;

		/// <summary>
		/// Create a model component with the given path.
		/// </summary>
		/// <param name="path">This path value is used to load the default mesh for this model component.</param>
		/// <returns></returns>
		explicit RenderComponent(const std::string& path) noexcept;

	public:
		friend class cereal::access;

		virtual ~RenderComponent() override = default;
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;

		/// <summary>
		/// This method allows you to change the model to render of this model component.
		/// </summary>
		/// <param name="newModel">The new model to use.</param>
		void SetModel(std::shared_ptr<Model> newModel);

		/// <summary>
		/// This method allows you to change the model to render of this model component.
		/// </summary>
		/// <param name="path">The path of the new model to use.</param>
		void SetModel(const std::string& path);

		/// <summary>
		///	This method allows you to retrieve the currently assigned model of this render component
		/// </summary>
		///	<returns>The currently assigned model. Note that this can be null</returns>
		[[nodiscard]] std::shared_ptr<Model> GetModel() const;

		/// <summary>
		/// This method allows you to change the material which is used for rendering.
		/// </summary>
		/// <param name="newMaterial">The new material to use for rendering.</param>
		void SetMaterial(std::weak_ptr<Material> newMaterial);

		/// <summary>
		///	This method allows you to retrieve the currently assigned material of this render component
		/// </summary>
		///	<returns>The currently assigned material. Note that this can be null</returns>
		[[nodiscard]] std::shared_ptr<Material> GetMaterial() const;

	protected:
		std::weak_ptr<TransformComponent> transformComponent;

		virtual void OnComponentAdded(std::shared_ptr<Component> addedComponent) override;

		virtual void OnComponentRemoved(std::shared_ptr<Component> removedComponent) override;

	private:
		std::shared_ptr<Model> model;
		std::weak_ptr<Material> material;

		/// <summary>
		/// Saves the data of this component to a archive 
		/// </summary>
		/// <param name="archive"> the archive that you want to save to </param>
		template <typename Archive>
		void Serialize(Archive& archive);
	};
}
