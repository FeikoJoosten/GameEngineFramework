#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/Asset.hpp"

#include <crossguid/guid.hpp>

namespace Engine {
	bool AssetRegistry::TryGetPathForGuid(const xg::Guid assetGuid, std::string& assetPath) const {
		for (const auto& [guid, path] : assetRegistryPathsByGuid) {
			if (guid == assetGuid) {
				assetPath = path;
				return true;
			}
		}
		return false;
	}

	bool AssetRegistry::TryGetGuidForPath(const std::string& assetPath, xg::Guid& assetGuid) const {
		for(const auto& [guid, path] : assetRegistryPathsByGuid) {
			if(path == assetPath) {
				assetGuid = guid;
				return true;
			}
		}
		
		return false;
	}

	bool AssetRegistry::TryRegisterAsset(const std::shared_ptr<Asset>& assetToRegister, const std::string& assetPath) {
		const std::map<xg::Guid, std::string>::iterator iterator = assetRegistryPathsByGuid.find(assetToRegister->GetGuid());

		if (iterator != assetRegistryPathsByGuid.end()) return false;

		assetToRegister->guid = xg::newGuid();
		assetRegistryPathsByGuid.insert({ assetToRegister->guid, assetPath });
		OnAssetRegisteredEvent(assetToRegister, assetPath);
		return true;
	}

	bool AssetRegistry::TryUnRegisterAsset(const xg::Guid& assetGuid) {
		const std::map<xg::Guid, std::string>::iterator iterator = assetRegistryPathsByGuid.find(assetGuid);

		if (iterator != assetRegistryPathsByGuid.end())	return false;

		OnAssetUnRegisteredEvent(iterator->first, assetGuid);
		assetRegistryPathsByGuid.erase(iterator);
		return true;
	}

	bool AssetRegistry::TryUpdatePathForGuid(const xg::Guid& assetGuid, const std::string& newAssetPath) {
		const std::map<xg::Guid, std::string>::iterator iterator = assetRegistryPathsByGuid.find(assetGuid);

		if (iterator != assetRegistryPathsByGuid.end())	return false;

		OnAssetMovedOrRenamedEvent(assetGuid, iterator->second, newAssetPath);
		iterator->second = newAssetPath;
		return true;
	}
}
