#include "Engine/Shader/ShaderStageAssetImporter.hpp"

namespace Engine {
	ShaderStageAssetImporter::ShaderStageAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<ShaderStage>(assetRegistry) {}

	const char* ShaderStageAssetImporter::GetDefaultAssetExtension() const {
		return ".shaderStage";
	}

	std::shared_ptr<Asset> ShaderStageAssetImporter::CreateAsset() const {
		return std::shared_ptr<ShaderStage>(new ShaderStage());
	}
}
