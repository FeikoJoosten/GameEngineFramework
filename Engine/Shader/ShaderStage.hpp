#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Shader/ShaderStageType.hpp"

#include <cereal/access.hpp>

namespace Engine {
	class ENGINE_API ShaderStage final : public Asset {
		friend cereal::access;
		friend class ShaderStageAssetImporter;

	public:
		ShaderStageType shaderStageType = ShaderStageType::Vertex;
		std::string shaderStageCode = {}; // Expected in hlsl format

	private:
		explicit ShaderStage() = default;

	public:
		virtual ~ShaderStage() override = default;
		ShaderStage(const ShaderStage& other) = delete;
		ShaderStage(ShaderStage&& other) noexcept = delete;
		ShaderStage& operator=(const ShaderStage& other) = delete;
		ShaderStage& operator=(ShaderStage&& other) noexcept = delete;

	private:

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void ShaderStage::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(shaderStageType),
			CEREAL_NVP(shaderStageCode)
		);
	}

}
