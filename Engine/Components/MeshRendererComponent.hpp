#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetReference.hpp"
#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Utility/Defines.hpp"

namespace Engine
{
	/// <summary>
	/// This component is used to keep track of the mesh and material combination to render.
	/// </summary>
	class ENGINE_API MeshRendererComponent : public RenderComponent
	{
		friend cereal::access;
		friend std::shared_ptr<MeshRendererComponent> Entity::AddComponent();

		AssetReference<Mesh> mesh {};

		explicit MeshRendererComponent() = default;

	public:
		Sharp::Event<std::shared_ptr<MeshRendererComponent>, std::shared_ptr<Mesh>, std::shared_ptr<Mesh>> OnMeshChangedEvent;

		virtual ~MeshRendererComponent() override = default;
		MeshRendererComponent(const MeshRendererComponent& other) = delete;
		MeshRendererComponent(MeshRendererComponent&& other) = delete;
		MeshRendererComponent& operator=(const MeshRendererComponent& other) = delete;
		MeshRendererComponent& operator=(MeshRendererComponent&& other) = delete;

		/// <summary>
		/// This method allows you to change the mesh to render.
		/// </summary>
		/// <param name="newMesh">The new mesh to use.</param>
		virtual void SetMesh(const std::shared_ptr<Mesh>& newMesh);

		/// <summary>
		///	This method allows you to retrieve the currently assigned mesh of this render component
		/// </summary>
		///	<returns>The currently assigned mesh. Note that this can be null</returns>
		[[nodiscard]] std::shared_ptr<Mesh> GetMesh() const;

	private:
		virtual void Render(const std::shared_ptr<CameraComponent>& camera) const override;

		template<class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void MeshRendererComponent::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<RenderComponent>(this)),
			CEREAL_NVP(mesh)
		);
	}
} //namespace Engine
