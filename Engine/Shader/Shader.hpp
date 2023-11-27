#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetReference.hpp"
#include "Engine/Shader/ShaderParameter.hpp"
#include "Engine/Shader/ShaderStage.hpp"
#include "Engine/Shader/ShaderStageType.hpp"

#include <cereal/access.hpp>
#include <cereal/types/map.hpp>

namespace Engine {
	class ENGINE_API Shader : public Asset {
		friend cereal::access;
		friend class ShaderAssetImporter;

		std::map<ShaderStageType, xg::Guid> shaderStageGuidByType {};
		std::vector<std::shared_ptr<ShaderParameter>> shaderParameters {};
		std::map<xg::Guid, AssetReference<ShaderStage>> loadedShaderStagesByGuid {};

		explicit Shader() = default;

	public:
		virtual ~Shader() override = default;
		Shader(const Shader& other) = delete;
		Shader(Shader&& other) noexcept = delete;
		Shader& operator=(const Shader& other) = delete;
		Shader& operator=(Shader&& other) noexcept = delete;

		void AddOrReplaceShaderStage(const std::shared_ptr<ShaderStage>& shaderStage);

		ENGINE_LOCAL std::shared_ptr<ShaderParameter> AddShaderParameter(const std::string& parameterName, ShaderParameterType parameterType, bool isGlobal);

		ENGINE_LOCAL void RemoveShaderParameter(const std::string& parameterName);

		ENGINE_LOCAL void ClearShaderParameters();

		bool TryGetShaderParameter(const std::string& parameterName, std::shared_ptr<ShaderParameter>& shaderParameter) const;

		[[nodiscard]] const std::vector<std::shared_ptr<ShaderParameter>>& GetShaderParameters() const;

		void GetShaderStages(std::vector<std::shared_ptr<ShaderStage>>& shaderStages) const;

	private:

		void LoadShaderStages();

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void Shader::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(shaderStageGuidByType),
			CEREAL_NVP(shaderParameters)
		);
	}
}