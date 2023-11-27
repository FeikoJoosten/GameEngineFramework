#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetReference.hpp"
#include "Engine/Components/Component.hpp"
#include "Engine/Material/Material.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <cereal/access.hpp>

namespace Engine {
	class CameraComponent;

	class ENGINE_API RenderComponent : public Component {
		friend Renderer;
		friend cereal::access;

		AssetReference<Material> material;

	protected:
		RenderComponent() = default;

	public:
		Sharp::Event<std::shared_ptr<RenderComponent>, std::shared_ptr<Material>, std::shared_ptr<Material>> OnMaterialChangedEvent;

		virtual ~RenderComponent() override = default;
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;

		void SetMaterial(const xg::Guid& newMaterialGuid);
		void SetMaterial(const std::shared_ptr<Material>& newMaterial);
		[[nodiscard]] std::shared_ptr<Material> GetMaterial() const;

	protected:

		virtual void Render(const std::shared_ptr<CameraComponent>& camera) const = 0;

	private:

		template<class Archive>
		void Serialize(Archive& archive);
	};

	template <typename Archive>
	void RenderComponent::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Component>(this)),
			CEREAL_NVP(material)
		);
	}
}
