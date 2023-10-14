#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetAssetImporter.hpp"
#include "Engine/AssetManagement/AssetImportSettingsAssetImporter.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/AssetManagement/EntityAssetImporter.hpp"
#include "Engine/AssetManagement/MaterialAssetImporter.hpp"
#include "Engine/AssetManagement/MeshAssetImporter.hpp"
#include "Engine/AssetManagement/ModelAssetImporter.hpp"
#include "Engine/AssetManagement/SceneAssetImporter.hpp"
#include "Engine/AssetManagement/TextureAssetImporter.hpp"
#include "Engine/Engine/Engine.hpp"

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
		std::shared_ptr<IAssetImporter> importerToUse;

		for (const std::shared_ptr<IAssetImporter>& assetImporter : assetImporters) {
			if (!assetImporter->SupportsAsset(asset)) continue;

			importerToUse = assetImporter;
			break;
		}
		if (!importerToUse) {
			DEBUG_ERROR("No importer found for asset, cannot write to path!")
			return;
		}

		const std::string extensionToUse = assetExtension != nullptr ? assetExtension : importerToUse->GetDefaultAssetExtension();
		if (!AssetRegistry::IsAssetRegistered(asset->GetGuid())) {
			if (!assetRegistry->TryRegisterAsset(asset, FullPathToPathInProject(fullPath), (asset->GetName() + extensionToUse).c_str())) {
				DEBUG_ERROR("Failed to process asset for: " + fullPath);
				return;
			}
		}

		WriteDataToFullPath(fullPath, asset->GetName(), asset, extensionToUse, serializationType, writeNameValuePair);
	}

	bool AssetManager::FileExists(const std::string& fullPath, const std::string& fileName, const bool isRelativeFromProject) const {
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

	bool AssetManager::TryGetAssetImporterForPath(const std::string& fullPath, std::shared_ptr<IAssetImporter>& outputAssetImporter) const {
		const std::string sanitizedFullPath = PathInProjectToFullPath(StringUtility::SanitizePath(fullPath));
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

		if (!file)
			throw std::runtime_error("Failed to find file!");

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
