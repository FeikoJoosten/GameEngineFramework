#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"

CEREAL_REGISTER_TYPE(Engine::Asset);

namespace Engine {
	Asset::Asset(std::string name) : name(std::move(name)) {
	}

	const std::string& Asset::GetName() const {
		return name;
	}

	void Asset::SetName(const std::string& newName) {
		name = newName;
	}

	const xg::Guid& Asset::GetGuid() const {
		return guid;
	}

	void Asset::Save() {
		std::string currentFilePath;
		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		if (std::string assetNameWithExtension; AssetRegistry::TryGetFullPathForGuid(guid, currentFilePath, assetNameWithExtension))
			assetManager->WriteAssetToFullPath(currentFilePath, shared_from_this());
		else
			DEBUG_ERROR("Failed to save asset, because it was not yet registered with the asset registry!");
	}
}
