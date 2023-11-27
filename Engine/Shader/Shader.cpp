#include "Engine/Shader/Shader.hpp"

#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Shader/ShaderStage.hpp"

#include <ranges>

CEREAL_REGISTER_TYPE(Engine::Shader);

namespace Engine
{
	void Shader::AddOrReplaceShaderStage(const std::shared_ptr<ShaderStage>& shaderStage) {
		const xg::Guid& shaderGuid = GetGuid();
		if (const std::map<ShaderStageType, xg::Guid>::iterator& iterator = shaderStageGuidByType.find(shaderStage->shaderStageType); iterator != shaderStageGuidByType.end())
			AssetRegistry::RemoveChildAssetFromAsset(shaderGuid, iterator->second);

		const xg::Guid& shaderStageGuid = shaderStage->GetGuid();
		if(!AssetRegistry::TryAddChildAssetToAsset(shaderGuid, shaderStageGuid)) return;

		shaderStageGuidByType.insert_or_assign(shaderStage->shaderStageType, shaderStageGuid);
		loadedShaderStagesByGuid.insert_or_assign(shaderStageGuid, shaderStage);
	}

	std::shared_ptr<ShaderParameter> Shader::AddShaderParameter(const std::string& parameterName, const ShaderParameterType parameterType, const bool isGlobal) {
		for (const std::shared_ptr<ShaderParameter>& shaderParameter : shaderParameters)
			if (shaderParameter->name == parameterName) return {}; // Duplicates are not allowed!

		std::shared_ptr<ShaderParameter> parameterToAdd = std::shared_ptr<ShaderParameter>(new ShaderParameter());
		parameterToAdd->name = parameterName;
		parameterToAdd->type = parameterType;
		parameterToAdd->isGlobal = isGlobal;
		shaderParameters.push_back(std::move(parameterToAdd));
		return shaderParameters.back();
	}

	void Shader::RemoveShaderParameter(const std::string& parameterName) {
		std::vector<std::shared_ptr<ShaderParameter>>::iterator removeLocation = shaderParameters.end();
		for (std::vector<std::shared_ptr<ShaderParameter>>::iterator iterator = shaderParameters.begin(); iterator != shaderParameters.end(); ++iterator) {
			if ((*iterator)->name != parameterName) continue;

			removeLocation = iterator;
			break;
		}

		if (removeLocation != shaderParameters.end()) shaderParameters.erase(removeLocation);
	}

	void Shader::ClearShaderParameters() {
		shaderParameters.clear();
	}

	bool Shader::TryGetShaderParameter(const std::string& parameterName, std::shared_ptr<ShaderParameter>& shaderParameter) const {
		bool foundParameter = false;
		for (const std::shared_ptr<ShaderParameter>& parameter : shaderParameters) {
			if (parameter->name != parameterName) continue;

			shaderParameter = parameter;
			foundParameter = true;
			break;
		}

		return foundParameter;
	}

	const std::vector<std::shared_ptr<ShaderParameter>>& Shader::GetShaderParameters() const {
		return shaderParameters;
	}

	void Shader::GetShaderStages(std::vector<std::shared_ptr<ShaderStage>>& shaderStages) const {
		for (const AssetReference<ShaderStage>& shaderStage : loadedShaderStagesByGuid | std::views::values)
			shaderStages.emplace_back(shaderStage);
	}

	void Shader::LoadShaderStages() {
		std::vector<xg::Guid> childGuids;
		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		if (!AssetRegistry::TryGetChildrenForAsset(GetGuid(), childGuids)) return;

		loadedShaderStagesByGuid.clear();

		for (xg::Guid childGuid : childGuids) {
			const std::shared_ptr<ShaderStage>& shaderStage = assetManager->LoadAsset<ShaderStage>(childGuid);
			if (!shaderStage) continue;

			loadedShaderStagesByGuid.insert_or_assign(childGuid, shaderStage);
		}

		if (shaderParameters.empty()) Renderer::Get()->TryExtractShaderParameters(std::static_pointer_cast<Shader>(shared_from_this()));
	}
}
