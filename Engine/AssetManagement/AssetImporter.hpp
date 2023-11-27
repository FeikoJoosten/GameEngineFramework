#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetImportSettings.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/Utility/StringUtility.hpp"

namespace Engine {
	class AssetImportSettings;

	template <typename AssetType>
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

		virtual std::shared_ptr<Asset> ImportAsset(const std::string& fullPath, const char* assetNameWithExtension) final override;

		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::string& fullPath, const char* assetNameWithExtension) final override;

		[[nodiscard]] virtual std::shared_ptr<Asset> LoadAsset(const xg::Guid& assetGuid) const final override;

		[[nodiscard]] virtual std::shared_ptr<AssetImportSettings> GetDefaultImportSettings() const override;

	protected:
		virtual void TryAppendAssetExtension(std::string& fileName);

		virtual std::shared_ptr<Asset> ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings);

		virtual std::shared_ptr<Asset> ImportAsset(const std::string& fullPath, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings);

		[[nodiscard]] virtual std::shared_ptr<Asset> LoadAsset(const xg::Guid& assetGuid, const std::shared_ptr<AssetImportSettings>& importSettings) const;
	};

	template <typename AssetType>
	AssetImporter<AssetType>::AssetImporter(std::shared_ptr<AssetRegistry> assetRegistry) : assetRegistry(std::move(assetRegistry)), supportedType(typeid(AssetType)) {
		static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");
	}

	template <typename AssetType>
	const char* AssetImporter<AssetType>::GetDefaultAssetExtension() const {
		return ".asset";
	}

	template <typename AssetType>
	bool AssetImporter<AssetType>::SupportsAsset(const std::shared_ptr<Asset>& assetToVerify) {
		return std::dynamic_pointer_cast<AssetType>(assetToVerify) != nullptr;
	}

	template <typename AssetType>
	bool AssetImporter<AssetType>::SupportsFile(const std::string& fullPath, const std::string& fileExtension) {
		return fileExtension == GetDefaultAssetExtension();
	}

	template <typename AssetType> bool AssetImporter<AssetType>::SupportsType(const type_info& typeInfo) {
		return typeInfo == supportedType;
	}

	template <typename AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::ImportAsset(const std::shared_ptr<Asset>& assetToImport) {
		std::string fullPath;
		std::string assetNameWithExtension;
		if (!assetToImport->GetGuid().isValid() || !AssetRegistry::TryGetFullPathForGuid(assetToImport->GetGuid(), fullPath, assetNameWithExtension)) {
			DEBUG_ERROR("Cannot import asset because it is not yet registerd!");
			return {};
		}

		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		std::shared_ptr<AssetImportSettings> importSettings;
		if (!assetToImport->GetGuid().isValid() || !AssetRegistry::TryGetImportSettingsForAsset(assetToImport->GetGuid(), importSettings)) {
			importSettings = GetDefaultImportSettings();
			importSettings->SetName(assetNameWithExtension);
			assetManager->WriteAssetToFullPath(fullPath, importSettings);
		}

		std::shared_ptr<Asset> importedAsset = ImportAsset(assetToImport, importSettings);
		if (!importedAsset) return {};

		bool isDirty = false;
		if (importSettings->GetName() != assetNameWithExtension) {
			importSettings->SetName(assetNameWithExtension);
			isDirty = true;
		}
		if (const xg::Guid& importedAssetGuid = importedAsset->GetGuid(); importSettings->relatedAssetGuid != importedAssetGuid) {
			importSettings->relatedAssetGuid = importedAsset->GetGuid();
			isDirty = true;
		}

		if (isDirty) assetManager->WriteAssetToFullPath(fullPath, importSettings);
		return importedAsset;
	}

	template <typename AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::ImportAsset(const std::string& fullPath, const char* assetNameWithExtension) {
		StringUtility::SanitizePath(fullPath);
		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();

		std::shared_ptr<AssetImportSettings> importSettings;
		if (xg::Guid assetGuid; !AssetRegistry::TryGetGuidForFullPath(fullPath, assetNameWithExtension, assetGuid) || !AssetRegistry::TryGetImportSettingsForAsset(assetGuid, importSettings)) {
			importSettings = GetDefaultImportSettings();
			importSettings->SetName(assetNameWithExtension);
			assetManager->WriteAssetToFullPath(fullPath, importSettings);
		}

		std::shared_ptr<Asset> importedAsset = ImportAsset(fullPath, assetNameWithExtension, importSettings);
		if (!importedAsset) return {};

		bool isDirty = false;
		if (importSettings->GetName() != assetNameWithExtension) {
			importSettings->SetName(assetNameWithExtension);
			isDirty = true;
		}
		if (const xg::Guid& importedAssetGuid = importedAsset->GetGuid(); importSettings->relatedAssetGuid != importedAssetGuid) {
			importSettings->relatedAssetGuid = importedAsset->GetGuid();
			isDirty = true;
		}

		if (isDirty) assetManager->WriteAssetToFullPath(fullPath, importSettings);
		return importedAsset;
	}

	template <typename AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::string& fullPath, const char* assetNameWithExtension) {
		StringUtility::SanitizePath(fullPath);
		const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
		const std::shared_ptr<IAssetImporter> importSettingsImporter = assetManager->GetAssetImporterForAssetType<AssetImportSettings>();
		const std::string importSettingsExtension = importSettingsImporter->GetDefaultAssetExtension();
		const std::string importSettingsFileName = assetNameWithExtension + importSettingsExtension;

		std::shared_ptr<AssetImportSettings> importSettings;
		const xg::Guid& assetGuid = assetToImport->GetGuid();
		if (!assetGuid.isValid() || !AssetRegistry::TryGetImportSettingsForAsset(assetGuid, importSettings)) {
			importSettings = GetDefaultImportSettings();
			importSettings->SetName(assetNameWithExtension);
			assetManager->WriteAssetToFullPath(fullPath, importSettings);
		}

		if (!assetGuid.isValid())
			assetRegistry->TryRegisterAsset(assetToImport, importSettings->GetGuid(), fullPath, assetNameWithExtension);

		std::shared_ptr<Asset> importedAsset = ImportAsset(assetToImport, importSettings);
		if (!importedAsset) return {};

		bool isDirty = false;
		if (importSettings->GetName() != assetNameWithExtension) {
			importSettings->SetName(assetNameWithExtension);
			isDirty = true;
		}
		if (const xg::Guid& importedAssetGuid = importedAsset->GetGuid(); importSettings->relatedAssetGuid != importedAssetGuid) {
			importSettings->relatedAssetGuid = importedAsset->GetGuid();
			isDirty = true;
		}

		if (isDirty) assetManager->WriteAssetToFullPath(fullPath, importSettings);
		return importedAsset;
	}

	template <typename AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::LoadAsset(const xg::Guid& assetGuid) const {
		std::shared_ptr<AssetImportSettings> importSettings;
		if (!assetRegistry->TryGetImportSettingsForAsset(assetGuid, importSettings)) return {};

		return LoadAsset(assetGuid, importSettings);
	}

	template <typename AssetType>
	std::shared_ptr<AssetImportSettings> AssetImporter<AssetType>::GetDefaultImportSettings() const {
		return std::make_shared<AssetImportSettings>();
	}

	template <typename AssetType>
	void AssetImporter<AssetType>::TryAppendAssetExtension(std::string& fileName) {
		const std::string assetExtension = GetDefaultAssetExtension();
		const size_t assetExtensionLength = assetExtension.length();

		if (const size_t fileNameLength = fileName.length(); fileNameLength < assetExtensionLength || fileName.compare(fileNameLength - assetExtensionLength, assetExtensionLength, assetExtension) != 0)
			fileName += assetExtension;
	}

	template <typename AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::ImportAsset(const std::shared_ptr<Asset>& assetToImport, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return assetToImport;
	}

	template <typename AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::ImportAsset(const std::string& fullPath, const char* assetNameWithExtension, const std::shared_ptr<AssetImportSettings>& importSettings) {
		return AssetManager::ReadDataFromFullPath<std::shared_ptr<Asset>>(fullPath, assetNameWithExtension);
	}

	template <typename AssetType>
	std::shared_ptr<Asset> AssetImporter<AssetType>::LoadAsset(const xg::Guid& assetGuid, const std::shared_ptr<AssetImportSettings>& importSettings) const {
		std::string fullPath;
		std::string assetNameWithExtension;
		if (!AssetRegistry::TryGetFullPathForGuid(assetGuid, fullPath, assetNameWithExtension)) {
			DEBUG_ERROR("Cannot load asset because it is not registered!");
			return {};
		}

		const std::shared_ptr<Asset>& loadedAsset = AssetManager::ReadDataFromFullPath<std::shared_ptr<Asset>>(fullPath, assetNameWithExtension);

		return loadedAsset;
	}
}

