#pragma once

#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetReference.hpp"
#include "Engine/Shader/Shader.hpp"

#include <cereal/access.hpp>

namespace Engine {

	class ENGINE_API Material : public Asset
	{
		friend cereal::access;
		friend class MaterialAssetImporter;

		AssetReference<Shader> shader;

		explicit Material() = default;
	public:
		Sharp::Event<const std::shared_ptr<Material>&, const std::shared_ptr<Shader>&, const std::shared_ptr<Shader>&> OnShaderChangedEvent;

		virtual ~Material() override = default;
		Material(const Material& other) = delete;
		Material(Material&& other) noexcept = delete;

		std::shared_ptr<Shader> GetShader() const;
		void SetShader(const xg::Guid& newShaderGuid);
		void SetShader(const std::shared_ptr<Shader>& newShader);

		Material& operator=(const Material& other) = delete;
		Material& operator=(Material&& other) noexcept = delete;

	private:

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void Material::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(shader)
		);
	}

}