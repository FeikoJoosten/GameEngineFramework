#include "Engine/Shader/ShaderAssetImporter.hpp"

namespace Engine {
	ShaderAssetImporter::ShaderAssetImporter(const std::shared_ptr<AssetRegistry>& assetRegistry) : AssetImporter<Shader>(assetRegistry) {}

	const char* ShaderAssetImporter::GetDefaultAssetExtension() const {
		return ".shader";
	}

	std::shared_ptr<Asset> ShaderAssetImporter::CreateAsset() const {
		return std::shared_ptr<Shader>(new Shader());
	}

	std::shared_ptr<Asset> ShaderAssetImporter::LoadAsset(const xg::Guid& assetGuid, const std::shared_ptr<AssetImportSettings>& importSettings) const {
		const std::shared_ptr<Asset>& loadedAsset = AssetImporter<Shader>::LoadAsset(assetGuid, importSettings);
		if (loadedAsset) std::static_pointer_cast<Shader>(loadedAsset)->LoadShaderStages();
		return loadedAsset;
	}
}
