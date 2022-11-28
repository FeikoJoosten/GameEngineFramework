#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Entity/Entity.hpp"

namespace Engine {
	class Material;
	class Model;
	class TransformComponent;

	class ENGINE_API RenderComponent : public Component {
		friend cereal::access;

		std::weak_ptr<TransformComponent> transformComponent;
		std::shared_ptr<Model> model;
		std::weak_ptr<Material> material;

	protected:
		RenderComponent() = default;

	public:

		virtual ~RenderComponent() override = default;
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;

		/// <summary>
		/// This method allows you to change the model to render of this model component.
		/// </summary>
		/// <param name="newModel">The new model to use.</param>
		virtual void SetModel(std::shared_ptr<Model> newModel);

		/// <summary>
		/// This method allows you to change the model to render of this model component.
		/// </summary>
		/// <param name="path">The path of the new model to use.</param>
		virtual void SetModel(const std::string& path);

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

		virtual void OnComponentAdded(std::shared_ptr<Component> addedComponent) override;

		virtual void OnComponentRemoved(std::shared_ptr<Component> removedComponent) override;

	private:
		template<class Archive>
		void Save(Archive& archive) const;

		template<class Archive>
		void Load(Archive& archive);
	};

	template <typename Archive>
	void RenderComponent::Save(Archive& archive) const {
		archive(
			CEREAL_NVP(cereal::base_class<Component>(this)),
			CEREAL_NVP(model),
			CEREAL_NVP(transformComponent)
		);
	}

	template <typename Archive>
	void RenderComponent::Load(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Component>(this)),
			CEREAL_NVP(model),
			CEREAL_NVP(transformComponent)
		);
	}
}
