#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetImportSettings.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/Utility/StringUtility.hpp"

namespace Engine {
	class AssetImportSettings;

	template <class AssetType>
	class ENGINE_API AssetImporter : public IAssetImporter {
	protected:
		std::shared_ptr<AssetRegistry> assetRegistry;
		const type_info& supportedType;

		explicit AssetImporter(std::shared_ptr<AssetRegistry> assetRegistry);

	public:
		AssetImporter() = delete;
		virtual ~AssetImporter() = default;
		AssetImporter(const AssetImporter& other) = delete;
		AssetImporter(AssetImporter&& other) = delete;
		AssetImporter& operator=(const AssetImporter& other) = delete;
		AssetImporter& operator=(AssetImporter&& other) = delete;

		[[nodiscard]] virtual const char* GetDefaultAssetExtension() const override;

		virtual bool SupportsAsset(const std::shared_ptr<Asset>& assetToVerify) override;

		virtual bool SupportsFile(const std::string& fullPath, const std::string& fileExtension) override;

		virtual bool SupportsType(const type_info& typeInfo) override;

		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport) final override;

		virtual std::shared_ptr<Asset> ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension) final override;

		[[nodiscard]] virtual std::shared_ptr<Asset> LoadAsset(const xg::Guid& assetGuid) const override;

	protected:
		virtual void TryAppendAssetExtension(std::string& fileName);

		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings);

		virtual std::shared_ptr<Asset> ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) = 0;

		virtual std::shared_ptr<AssetImportSettings> GetDefaultImportSettings();
	};

	template <class AssetType>
	AssetImporter<AssetType>::AssetImporter(std::shared_ptr<AssetRegistry> assetRegistry) : assetRegistry(std::move(assetRegistry)), supportedType(typeid(AssetType)) {
		static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");
	}

	template <class AssetType>
	const char* AssetImporter<AssetType>::GetDefaultAssetExtension() const {
		return ".asset";
	}

	template <class AssetType>
	bool AssetImporter<AssetType>::SupportsAsset(const std::shared_ptr<Asset>& assetToVerify) {
		return std::dynamic_pointer_cast<AssetType>(assetToVerify) != nullptr;
	}

	template <class AssetType>
	bool AssetImporter<AssetType>::SupportsFile(const std::string& fullPath, const std::string& fileExtension) {
		return fileExtension == GetDefaultAssetExtension();
	}

	template <class AssetType> bool AssetImporter<AssetType>::SupportsType(const type_info& typeInfo) {
		return typeInfo == supportedType;
	}

	template <class AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::ImportAsset(const std::shared_ptr<Asset>& assetToImport) {
		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		const std::shared_ptr<AssetImporter> importSettingsImporter = std::reinterpret_pointer_cast<AssetImporter>(assetManager->GetAssetImporterForAssetType<AssetImportSettings>());
		const std::string importSettingsExtension = importSettingsImporter->GetDefaultAssetExtension();

		std::shared_ptr<AssetImportSettings> importSettings = {};
		std::string pathInProject;
		std::string assetNameWithExtension;
		if (assetToImport->GetGuid().isValid() && AssetRegistry::TryGetPathForGuid(assetToImport->GetGuid(), pathInProject, assetNameWithExtension))
			importSettings = assetManager->ReadDataFromPath<std::shared_ptr<AssetImportSettings>>(pathInProject, assetNameWithExtension + importSettingsExtension);

		if (!importSettings) importSettings = GetDefaultImportSettings();
		std::shared_ptr<Asset> importedAsset = ImportAsset(assetToImport, importSettings);
		if (!importedAsset) return {};

		// child assets should never generate import settings until a valid nested import settings method is figured out
		if (xg::Guid parentGuid; AssetRegistry::TryGetParentForAsset(importedAsset->GetGuid(), parentGuid)) return importedAsset;

		bool isDirty = false;
		if (const std::string& importedAssetName = importedAsset->GetName(); importSettings->GetName() != importedAssetName) {
			importSettings->SetName(importedAssetName);
			isDirty = true;
		}
		if (const xg::Guid& importedAssetGuid = importedAsset->GetGuid(); importSettings->relatedAssetGuid != importedAssetGuid) {
			importSettings->relatedAssetGuid = importedAsset->GetGuid();
			isDirty = true;
		}

		if (isDirty) assetManager->WriteAssetToPath(pathInProject, importSettings, importSettingsExtension.c_str());
		return importedAsset;
	}

	template <class AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::ImportAsset(const std::string& pathInProject, const char* assetNameWithExtension) {
		StringUtility::SanitizePath(pathInProject);
		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		const std::shared_ptr<AssetImporter> importSettingsImporter = std::reinterpret_pointer_cast<AssetImporter>(assetManager->GetAssetImporterForAssetType<AssetImportSettings>());
		const std::string importSettingsExtension = importSettingsImporter->GetDefaultAssetExtension();

		std::shared_ptr<AssetImportSettings> importSettings = assetManager->ReadDataFromPath<std::shared_ptr<AssetImportSettings>>(pathInProject, assetNameWithExtension + importSettingsExtension);
		if (!importSettings) importSettings = GetDefaultImportSettings();
		std::shared_ptr<Asset> importedAsset = ImportAsset(pathInProject, assetNameWithExtension, importSettings);
		if (!importedAsset) return {};

		// child assets should never generate import settings until a valid nested import settings method is figured out
		if (xg::Guid parentGuid; AssetRegistry::TryGetParentForAsset(importedAsset->GetGuid(), parentGuid)) return importedAsset;

		bool isDirty = false;
		if (const std::string& importedAssetName = importedAsset->GetName(); importSettings->GetName() != importedAssetName) {
			importSettings->SetName(importedAssetName);
			isDirty = true;
		}
		if (const xg::Guid& importedAssetGuid = importedAsset->GetGuid(); importSettings->relatedAssetGuid != importedAssetGuid) {
			importSettings->relatedAssetGuid = importedAsset->GetGuid();
			isDirty = true;
		}

		if (isDirty) assetManager->WriteAssetToPath(pathInProject, importSettings, importSettingsExtension.c_str());
		return importedAsset;
	}

	template <class AssetType>
	void AssetImporter<AssetType>::TryAppendAssetExtension(std::string& fileName) {
		const std::string assetExtension = GetDefaultAssetExtension();
		const size_t assetExtensionLength = assetExtension.length();

		if (const size_t fileNameLength = fileName.length(); fileNameLength < assetExtensionLength || fileName.compare(fileNameLength - assetExtensionLength, assetExtensionLength, assetExtension) != 0)
			fileName += assetExtension;
	}

	template <class AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return assetToImport;
	}

	template <class AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::LoadAsset(const xg::Guid& assetGuid) const {
		const std::shared_ptr<AssetManager>& assetManager = AssetManager::Get();
		std::string pathInProject;
		std::string assetNameWithExtension;
		if (!AssetRegistry::TryGetPathForGuid(assetGuid, pathInProject, assetNameWithExtension)) {
			DEBUG_ERROR("Cannot load asset because it is not registered!");
			return {};
		}

		const std::shared_ptr<Asset>& loadedAsset = assetManager->ReadDataFromPath<std::shared_ptr<Asset>>(pathInProject, assetNameWithExtension);

		// If we load as the AssetType class, cereal throws an exception when loading as it cannot upcast from AssetType --> Asset?
		return loadedAsset;
	}

	template <class AssetType>
	std::shared_ptr<AssetImportSettings> AssetImporter<AssetType>::GetDefaultImportSettings() {
		return std::make_shared<AssetImportSettings>();
	}
}

