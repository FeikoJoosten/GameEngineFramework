#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"

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

	const std::string& Asset::GetDefaultExtension() const {
		return extension;
	}

	void Asset::Save() {
		std::string currentFilePath;
		std::string assetName;
		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		if (assetManager->GetAssetRegistry()->TryGetPathForGuid(guid, currentFilePath, assetName))
			assetManager->WriteDataToFullPath(currentFilePath, shared_from_this());
		else
			DEBUG_ERROR("Failed to save asset, because it was not yet registered with the asset registry!");
	}
}
