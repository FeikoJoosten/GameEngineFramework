#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Engine {
	class Material;

	class ENGINE_API RenderComponent : public Component {
		friend Renderer;
		friend cereal::access;

		std::weak_ptr<TransformComponent> transformComponent;
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

		// Invoked by Renderer
		virtual void Render() const;

	private:
		template<class Archive>
		void Serialize(Archive& archive);
	};

	template <typename Archive>
	void RenderComponent::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Component>(this)),
			CEREAL_NVP(transformComponent)
		);
	}
}
