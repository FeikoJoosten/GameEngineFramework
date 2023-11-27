#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetImportSettingsAssetImporter.hpp"
#include "Engine/Utility/StringUtility.hpp"

#include <crossguid/guid.hpp>
#include <nameof.hpp>

namespace Engine {
	const std::string ASSET_REGISTRY_PATH = static_cast<std::string>(NAMEOF_SHORT_TYPE(AssetRegistry)) + "/";

	void AssetRegistry::AssetMetaInfo::AddChildGuid(const xg::Guid childGuid) {
		if (std::ranges::find(childGuids, childGuid) != childGuids.end()) return;

		childGuids.push_back(childGuid);
	}

	bool AssetRegistry::AssetMetaInfo::operator==(const AssetMetaInfo& other) const {
		if (guid != other.guid) return false;
		if (parentGuid != other.parentGuid) return false;
		if (childGuids != other.childGuids) return false;
		if (fullPath != other.fullPath) return false;
		if (assetNameWithExtension != other.assetNameWithExtension) return false;

		return true;
	}

	bool AssetRegistry::AssetMetaInfo::operator!=(const AssetMetaInfo& other) const {
		return !operator==(other);
	}

	bool AssetRegistry::IsAssetRegistered(const xg::Guid& assetGuid) {
		if (AssetMetaInfo outputMetaInfo; TryLoadAssetMetaInfoForGuid(assetGuid, outputMetaInfo))
			return true;

		return false;
	}

	bool AssetRegistry::TryGetFullPathForGuid(const xg::Guid& assetGuid, std::string& fullPath, std::string& assetNameWithExtension) {
		if (AssetMetaInfo outputMetaInfo; TryLoadAssetMetaInfoForGuid(assetGuid, outputMetaInfo)) {
			fullPath = outputMetaInfo.fullPath;
			assetNameWithExtension = outputMetaInfo.assetNameWithExtension;
			return true;
		}

		return false;
	}

	bool AssetRegistry::TryGetGuidForPath(const std::string& pathInProject, const char* assetNameWithExtension, xg::Guid& assetGuid) {
		return TryGetGuidForFullPath(AssetManager::PathInProjectToFullPath(pathInProject), assetNameWithExtension, assetGuid);
	}

	bool AssetRegistry::TryGetGuidForFullPath(const std::string& fullPath, const char* assetNameWithExtension, xg::Guid& assetGuid) {
		std::string sanitizedPath = StringUtility::SanitizePath(fullPath);

		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		const std::shared_ptr<IAssetImporter> assetImportSettingsImporter = assetManager->GetAssetImporter<AssetImportSettingsAssetImporter>();
		const std::shared_ptr<AssetImportSettings> importSettings = std::static_pointer_cast<AssetImportSettings>(AssetManager::ReadDataFromFullPath<std::shared_ptr<Asset>>(fullPath, std::string(assetNameWithExtension).append(assetImportSettingsImporter->GetDefaultAssetExtension())));
		if (!importSettings || !importSettings->relatedAssetGuid.isValid()) { // Fail safe in case the asset does not generate import settings
			// This will end up being extremely slow if the project has thousands, if not already at hundreds, of assets. Is this really needed if import settings are always generated?
			for (const std::string assetRegistryFolder = AssetManager::GetProjectRoot() + AssetManager::GetNativeFolder() + ASSET_REGISTRY_PATH;
				const std::filesystem::directory_entry & directoryEntry : std::filesystem::directory_iterator { assetRegistryFolder }) {
				std::filesystem::path directoryPath = directoryEntry.path();
				std::filesystem::path folderName = directoryPath.filename();
				AssetMetaInfo assetMetaInfo = AssetManager::ReadDataFromFullPath<AssetMetaInfo>(directoryPath.string() + "/", folderName.generic_string());
				if (!assetMetaInfo.guid.isValid())	continue; // Do we need to error this?
				if (assetMetaInfo.assetNameWithExtension != assetNameWithExtension) continue;
				if (assetMetaInfo.fullPath != fullPath) continue;

				assetGuid = assetMetaInfo.guid;
				return true;
			}

			return false;
		}

		assetGuid = importSettings->relatedAssetGuid;
		return true;
	}

	bool AssetRegistry::TryRegisterAsset(const std::shared_ptr<Asset>& assetToRegister, const xg::Guid& importSettingsGuid, const std::string& fullPath, const char* assetNameWithExtension) {
		if (!assetToRegister) {
			DEBUG_ERROR("The asset you tried to register is null!");
			return false;
		}

		AssetMetaInfo outputMetaInfo;
		if (const xg::Guid& assetGuid = assetToRegister->GetGuid(); assetGuid.isValid() && TryLoadAssetMetaInfoForGuid(assetGuid, outputMetaInfo)) return true; // Already registered, no need to return false

		assetToRegister->guid = xg::newGuid();
		outputMetaInfo.assetNameWithExtension = assetNameWithExtension;
		outputMetaInfo.fullPath = fullPath;
		outputMetaInfo.guid = assetToRegister->guid;
		if (std::dynamic_pointer_cast<AssetImportSettings>(assetToRegister) && !importSettingsGuid.isValid()) outputMetaInfo.importSettingsGuid = assetToRegister->guid;
		else outputMetaInfo.importSettingsGuid = importSettingsGuid;
		SaveAssetMetaInfo(outputMetaInfo);

		OnAssetRegisteredEvent(assetToRegister, fullPath);
		return true;
	}

	bool AssetRegistry::TryUnRegisterAsset(const xg::Guid& assetGuid) {
		AssetMetaInfo outputMetaInfo;
		if (!TryLoadAssetMetaInfoForGuid(assetGuid, outputMetaInfo)) return false;

		OnAssetUnRegisteredEvent(outputMetaInfo.guid, outputMetaInfo.assetNameWithExtension);
		DeleteAssetMetaInfo(outputMetaInfo);

		return true;
	}

	bool AssetRegistry::TryUpdatePathForGuid(const xg::Guid& assetGuid, const std::string& newFullPath, const char* newAssetNameWithExtension) {
		AssetMetaInfo outputMetaInfo;
		if (!TryLoadAssetMetaInfoForGuid(assetGuid, outputMetaInfo)) return false;

		const std::string oldFullPath = outputMetaInfo.fullPath;
		const std::string oldAssetName = outputMetaInfo.assetNameWithExtension;
		outputMetaInfo.fullPath = newFullPath;
		outputMetaInfo.assetNameWithExtension = newAssetNameWithExtension; // TODO: Verify the incoming name has an extension appended to it
		SaveAssetMetaInfo(outputMetaInfo);

		// TODO: Should this also move the import settings?

		OnAssetMovedOrRenamedEvent(assetGuid, oldFullPath, newFullPath, oldAssetName, newAssetNameWithExtension);
		return true;
	}

	bool AssetRegistry::TryWriteCustomDataToAsset(const xg::Guid& assetGuid, const std::shared_ptr<Asset>& customData, const std::string& dataExtension) {
		if (!IsAssetRegistered(assetGuid)) {
			DEBUG_ERROR("Failed to add asset to parent as the parent was not yet registerd");
			return false;
		}

		AssetManager::Get()->WriteDataToFullPath(GetAssetRegistryPathForGuid(assetGuid), assetGuid, customData, dataExtension, Json);
		return true;
	}

	bool AssetRegistry::TryLoadCustomDataFromAsset(const xg::Guid& assetGuid, const std::string& dataExtension, std::shared_ptr<Asset>& customData) {
		if (!IsAssetRegistered(assetGuid)) {
			DEBUG_ERROR("Failed to load custom data as the guid is unknown");
			return false;
		}

		customData = AssetManager::ReadDataFromFullPath<std::shared_ptr<Asset>>(GetAssetRegistryPathForGuid(assetGuid), assetGuid.str() + dataExtension);
		return static_cast<bool>(customData);
	}

	bool AssetRegistry::TryAddChildAssetToAsset(const xg::Guid& parentGuid, const xg::Guid& childGuid) {
		AssetMetaInfo parentAssetMetaInfo;
		if (!TryLoadAssetMetaInfoForGuid(parentGuid, parentAssetMetaInfo)) {
			DEBUG_ERROR("Failed to add asset to parent as the parent was not yet registerd");
			return false;
		}

		for (const xg::Guid& knownChildGuid : parentAssetMetaInfo.childGuids) {
			if (knownChildGuid == childGuid) {
				//DEBUG_WARNING("Not adding asset to parent as the parent already has this asset as a direct child"); // No need to log, this just creates clutter in the logs
				return false;
			}
		}

		AssetMetaInfo assetToAddAssetMetaInfo;
		if (!TryLoadAssetMetaInfoForGuid(childGuid, assetToAddAssetMetaInfo)) {
			DEBUG_ERROR("Failed to add asset to parent as the asset was not yet registerd");
			return false;
		}

		parentAssetMetaInfo.childGuids.push_back(childGuid);
		assetToAddAssetMetaInfo.parentGuid = parentGuid;
		SaveAssetMetaInfo(parentAssetMetaInfo);
		SaveAssetMetaInfo(assetToAddAssetMetaInfo);
		return true;
	}

	void AssetRegistry::RemoveChildAssetFromAsset(const xg::Guid& parentGuid, const xg::Guid& childGuid) {
		AssetMetaInfo parentAssetMetaInfo;
		if (!TryLoadAssetMetaInfoForGuid(parentGuid, parentAssetMetaInfo)) {
			DEBUG_ERROR("Failed to remove asset from parent as the parent was not yet registerd");
			return;
		}

		const std::ranges::borrowed_iterator_t<std::vector<xg::Guid>&> iterator = std::ranges::find(parentAssetMetaInfo.childGuids, childGuid);
		if (iterator == parentAssetMetaInfo.childGuids.end()) return;


		AssetMetaInfo assetToAddAssetMetaInfo;
		if (!TryLoadAssetMetaInfoForGuid(childGuid, assetToAddAssetMetaInfo)) {
			DEBUG_ERROR("Failed to remove asset from parent as the asset was not yet registerd");
			return;
		}

		parentAssetMetaInfo.childGuids.erase(iterator);
		assetToAddAssetMetaInfo.parentGuid = xg::Guid();
		SaveAssetMetaInfo(parentAssetMetaInfo);
		SaveAssetMetaInfo(assetToAddAssetMetaInfo);
	}

	bool AssetRegistry::TryAssignGuidToAsset(const std::shared_ptr<Asset>& assetToUpdate, const xg::Guid& assetGuid) {
		if (!assetGuid.isValid()) {
			DEBUG_ERROR("Tried to assign invalid guid to asset!");
			return false;
		}

		assetToUpdate->guid = assetGuid;
		return true;
	}

	bool AssetRegistry::TryGetParentForAsset(const xg::Guid& childGuid, xg::Guid& parentGuid) {
		AssetMetaInfo childMetaInfo;
		if (!TryLoadAssetMetaInfoForGuid(childGuid, childMetaInfo)) return false;

		parentGuid = childMetaInfo.parentGuid;
		return true;
	}

	bool AssetRegistry::TryGetChildrenForAsset(const xg::Guid& parentGuid, std::vector<xg::Guid>& childGuids) {
		AssetMetaInfo parentMetaInfo;
		if (!TryLoadAssetMetaInfoForGuid(parentGuid, parentMetaInfo)) return false;

		for (xg::Guid childGuid : parentMetaInfo.childGuids) {
			if (std::ranges::find(childGuids, childGuid) != childGuids.end()) continue;

			childGuids.push_back(childGuid);
		}

		return true;
	}

	bool AssetRegistry::TryGetImportSettingsForAsset(const xg::Guid& assetGuid,	std::shared_ptr<AssetImportSettings>& importSettings) {
		AssetMetaInfo metaInfo;
		if (!TryLoadAssetMetaInfoForGuid(assetGuid, metaInfo)) {
			DEBUG_DEBUG("Cannot retrieve import settings, because the asset guid is unknown!");
			return false;
		}

		if (!metaInfo.importSettingsGuid.isValid())
			return false; // Should this log an error?

		if (!TryLoadAssetMetaInfoForGuid(metaInfo.importSettingsGuid, metaInfo)) {
			DEBUG_DEBUG("Cannot retrieve import settings, because the import settings cannot be found!");
			return false;
		}

		importSettings = std::static_pointer_cast<AssetImportSettings>(AssetManager::ReadDataFromFullPath<std::shared_ptr<Asset>>(metaInfo.fullPath, metaInfo.assetNameWithExtension));
		return static_cast<bool>(importSettings);
	}

	std::string AssetRegistry::GetAssetRegistryPathForGuid(const xg::Guid& assetGuid) {
		return AssetManager::GetProjectRoot() + AssetManager::GetNativeFolder() + ASSET_REGISTRY_PATH + assetGuid.str() + "/";
	}

	bool AssetRegistry::TryLoadAssetMetaInfoForGuid(const xg::Guid& assetGuid, AssetMetaInfo& outputMetaInfo) {
		outputMetaInfo = AssetManager::ReadDataFromFullPath<AssetMetaInfo>(GetAssetRegistryPathForGuid(assetGuid), assetGuid);
		return outputMetaInfo.guid.isValid();
	}

	void AssetRegistry::SaveAssetMetaInfo(const AssetMetaInfo& assetMetaInfo) {
		const std::string objectGuid = assetMetaInfo.guid;
		AssetManager::Get()->WriteDataToFullPath(GetAssetRegistryPathForGuid(assetMetaInfo.guid), assetMetaInfo.guid, assetMetaInfo);
	}

	void AssetRegistry::DeleteAssetMetaInfo(const AssetMetaInfo& assetMetaInfo) {
		AssetManager::Get()->DeleteFile(GetAssetRegistryPathForGuid(assetMetaInfo.guid), assetMetaInfo.guid, true);
	}

}
