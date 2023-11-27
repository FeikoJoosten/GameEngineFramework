#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetAssetImporter.hpp"
#include "Engine/AssetManagement/AssetImportSettingsAssetImporter.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/Entity/EntityAssetImporter.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Material/MaterialAssetImporter.hpp"
#include "Engine/Mesh/MeshAssetImporter.hpp"
#include "Engine/Model/ModelAssetImporter.hpp"
#include "Engine/Scene/SceneAssetImporter.hpp"
#include "Engine/Shader/ShaderAssetImporter.hpp"
#include "Engine/Shader/ShaderStageAssetImporter.hpp"
#include "Engine/Texture/TextureAssetImporter.hpp"

#include <nameof.hpp>

namespace Engine {
	AssetManager::AssetManager() {
		assetRegistry = std::shared_ptr<AssetRegistry>(new AssetRegistry());
		fileWatcher = std::unique_ptr<EngineProjectFileWatcher>(new EngineProjectFileWatcher(projectRoot + resourcesFolder, assetRegistry));

		assetImporters.push_back(std::shared_ptr<AssetImportSettingsAssetImporter>(new AssetImportSettingsAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<EntityAssetImporter>(new EntityAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<MaterialAssetImporter>(new MaterialAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<MeshAssetImporter>(new MeshAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<ModelAssetImporter>(new ModelAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<SceneAssetImporter>(new SceneAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<ShaderAssetImporter>(new ShaderAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<ShaderStageAssetImporter>(new ShaderStageAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<TextureAssetImporter>(new TextureAssetImporter(assetRegistry)));
		assetImporters.push_back(std::shared_ptr<AssetAssetImporter>(new AssetAssetImporter(assetRegistry))); // Register as last so type specific importers have priority
	}

	std::shared_ptr<AssetManager> AssetManager::Get() {
		return Engine::GetAssetManager();
	}

	void AssetManager::WriteAssetToPath(const std::string& pathInProject, const std::shared_ptr<Asset>& asset, const char* assetExtension, const AssetSerializationType& serializationType, const bool writeNameValuePair) {
		WriteAssetToFullPath(projectRoot + resourcesFolder + pathInProject, asset, assetExtension, serializationType, writeNameValuePair);
	}

	void AssetManager::WriteAssetToFullPath(const std::string& fullPath, const std::shared_ptr<Asset>& asset, const char* assetExtension, const AssetSerializationType& serializationType, const bool writeNameValuePair) {
		std::string extensionToUse;
		std::shared_ptr<IAssetImporter> importerToUse;
		for (const std::shared_ptr<IAssetImporter>& assetImporter : assetImporters) {
			if (!assetImporter->SupportsAsset(asset)) continue;

			importerToUse = assetImporter;
			break;
		}

		if(!assetExtension) {
			if (!importerToUse) {
				DEBUG_ERROR("No importer found for asset, cannot write to path! Please ensure an importer exists for the asset type or provide an asset extension!");
				return;
			}

			extensionToUse = importerToUse->GetDefaultAssetExtension();
		} else extensionToUse = assetExtension;

		if (!AssetRegistry::IsAssetRegistered(asset->GetGuid())) {
			std::shared_ptr<AssetImportSettings> importSettings = std::dynamic_pointer_cast<AssetImportSettings>(asset);
			if (!importSettings && !AssetRegistry::TryGetImportSettingsForAsset(asset->GetGuid(), importSettings)) {
				DEBUG_DEBUG("No import settings found, trying to import asset");
				if (importerToUse && importerToUse->ImportAsset(asset, fullPath, (asset->GetName() + extensionToUse).c_str()))
					AssetRegistry::TryGetImportSettingsForAsset(asset->GetGuid(), importSettings);

				if (!importSettings) {
					DEBUG_ERROR("Failed to processet asset, as it doesn't have import settings generated!");
					return;
				}
			}

			if (!AssetRegistry::IsAssetRegistered(asset->GetGuid()) && !assetRegistry->TryRegisterAsset(asset, importSettings->GetGuid(), fullPath, (asset->GetName() + extensionToUse).c_str())) {
				DEBUG_ERROR("Failed to process asset for: " + fullPath);
				return;
			}
		}

		WriteDataToFullPath(fullPath, asset->GetName(), asset, extensionToUse, serializationType, writeNameValuePair);
	}

	bool AssetManager::FileExists(const std::string& fullPath, const std::string& fileName, const bool isRelativeFromProject) {
		return FileExists(isRelativeFromProject ? projectRoot + resourcesFolder + fileName : fullPath + "/" + fileName);
	}

	bool AssetManager::FileExists(const std::string& fullPath) {
		std::ifstream file(StringUtility::SanitizePath(fullPath));

		const bool fileExists = file.is_open();
		file.close();

		return fileExists;
	}

	std::string AssetManager::GetDirectoryFromPath(const std::string& path) {
		const std::filesystem::path directoryPath { path };
		
		return StringUtility::SanitizePath(directoryPath.parent_path().generic_string() + "/");
	}

	const std::string& AssetManager::GetProjectRoot() {
		return projectRoot;
	}

	std::shared_ptr<AssetRegistry> AssetManager::GetAssetRegistry() {
		return assetRegistry;
	}

	std::vector<std::shared_ptr<IAssetImporter>> AssetManager::GetAllAssetImporters() const {
		return assetImporters;
	}

	bool AssetManager::TryGetAssetImporterForPath(const std::string& pathInProject, std::shared_ptr<IAssetImporter>& outputAssetImporter) const {
		const std::string sanitizedFullPath = PathInProjectToFullPath(StringUtility::SanitizePath(pathInProject));
		const std::filesystem::path path = sanitizedFullPath;
		const std::string& genericExtension = path.extension().generic_string();

		for (const std::shared_ptr<IAssetImporter>& assetImporter : assetImporters) {
			if (!assetImporter->SupportsFile(sanitizedFullPath, genericExtension)) continue;

			outputAssetImporter = assetImporter;
			if (assetImporter) return true;
		}

		return false;
	}

	std::vector<char> AssetManager::ReadRawFile(const std::string& fileName, const int fileOpenMode) {
		std::ifstream file(projectRoot + resourcesFolder + fileName, fileOpenMode);

		if (!file.good()) return {};
		if (!file.is_open())
			throw std::runtime_error("Failed to open file!");

		file.seekg(0, std::ifstream::end);
		const std::streampos fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0, std::ifstream::beg);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	void AssetManager::DeleteFile(const std::string& fullPath, const std::string& fileName, const bool isRelativeFromProject) const {
		if(FileExists(fullPath, fileName, isRelativeFromProject))
			std::remove((isRelativeFromProject ? projectRoot + resourcesFolder + fileName : StringUtility::SanitizePath(fullPath) + "/" + fileName).c_str());
	}

	std::string AssetManager::PathInProjectToFullPath(const std::string& pathInProject) {
		return projectRoot + resourcesFolder + StringUtility::SanitizePath(pathInProject);
	}

	std::string AssetManager::FullPathToPathInProject(const std::string& fullPath) {
		return StringUtility::SanitizePath(fullPath).substr(projectRoot.length() + resourcesFolder.length());
	}

	std::string& AssetManager::GetNativeFolder() {
		return nativeFolder;
	}

	// TODO: Replace with self managed update subscribe in FileWatcher itself
	void AssetManager::UpdateFileWatcher() const {
		fileWatcher->Update();
	}
}
