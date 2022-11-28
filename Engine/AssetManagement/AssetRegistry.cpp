#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/Asset.hpp"

#include <crossguid/guid.hpp>

namespace Engine {
	bool AssetRegistry::TryGetPathForGuid(const xg::Guid& assetGuid, std::string& pathInProject, std::string& assetName) const {
		for (const auto& [guid, path] : assetRegistryPathsByGuid) {
			if (guid == assetGuid) {
				pathInProject = path.first;
				assetName = path.second;
				return true;
			}
		}
		return false;
	}

	bool AssetRegistry::TryGetGuidForPath(const std::string& assetPath, const std::string& assetName, xg::Guid& assetGuid) const {
		for(const auto& [guid, path] : assetRegistryPathsByGuid) {
			if(path.first == assetPath && path.second == assetName) {
				assetGuid = guid;
				return true;
			}
		}
		
		return false;
	}

	bool AssetRegistry::TryRegisterAsset(const std::shared_ptr<Asset>& assetToRegister, const std::string& pathInProject) {
		const std::map<xg::Guid, std::pair<std::string, std::string>>::iterator iterator = assetRegistryPathsByGuid.find(assetToRegister->GetGuid());

		if (iterator != assetRegistryPathsByGuid.end()) return false;

		assetToRegister->guid = xg::newGuid();
		assetRegistryPathsByGuid.insert({ assetToRegister->guid, { pathInProject, assetToRegister->GetName() } });
		OnAssetRegisteredEvent(assetToRegister, pathInProject);
		return true;
	}

	bool AssetRegistry::TryUnRegisterAsset(const xg::Guid& assetGuid) {
		const std::map<xg::Guid, std::pair<std::string, std::string>>::iterator iterator = assetRegistryPathsByGuid.find(assetGuid);

		if (iterator != assetRegistryPathsByGuid.end())	return false;

		OnAssetUnRegisteredEvent(iterator->first, assetGuid);
		assetRegistryPathsByGuid.erase(iterator);
		return true;
	}

	bool AssetRegistry::TryUpdatePathForGuid(const xg::Guid& assetGuid, const std::string& newPathInProject, const std::string& newAssetName) {
		const std::map<xg::Guid, std::pair<std::string, std::string>>::iterator iterator = assetRegistryPathsByGuid.find(assetGuid);

		if (iterator != assetRegistryPathsByGuid.end())	return false;

		iterator->second.first = newPathInProject;
		iterator->second.second = newAssetName;
		OnAssetMovedOrRenamedEvent(assetGuid, iterator->second.first, newPathInProject, iterator->second.second, newAssetName);
		return true;
	}

	bool AssetRegistry::TryAssignGuidToAsset(const std::shared_ptr<Asset>& assetToUpdate, const xg::Guid& assetGuid) {
		const std::map<xg::Guid, std::pair<std::string, std::string>>::iterator iterator = assetRegistryPathsByGuid.find(assetGuid);

		if (iterator == assetRegistryPathsByGuid.end())	return false;
		if (iterator->second.second != assetToUpdate->GetName()) return false;

		assetToUpdate->guid = assetGuid; // No need to invoke event, as the asset itself was out of sync
		return true;
	}
}
