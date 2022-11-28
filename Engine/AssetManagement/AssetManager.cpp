#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/AssetManagement/ModelImporter.hpp"
#include "Engine/AssetManagement/SceneImporter.hpp"
#include "Engine/Engine/Engine.hpp"

#include <nameof.hpp>

namespace Engine {
	const std::string assetRegistryPath = static_cast<std::string>(NAMEOF_SHORT_TYPE(AssetRegistry));

	AssetManager::AssetManager() {
		assetRegistry = ReadDataFromFullPath<std::shared_ptr<AssetRegistry>>(projectRoot + nativeFolder, assetRegistryPath);
		if (!assetRegistry) {
			assetRegistry = std::shared_ptr<AssetRegistry>(new AssetRegistry());
			SaveAssetRegistry();
		}

		assetRegistry->OnAssetRegisteredEvent += Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetRegisteredEvent);
		assetRegistry->OnAssetUnRegisteredEvent += Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetUnRegisteredEvent);
		assetRegistry->OnAssetMovedOrRenamedEvent += Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetMovedOrRenamedEvent);

		fileWatcher = std::unique_ptr<EngineProjectFileWatcher>(new EngineProjectFileWatcher(projectRoot + resourcesFolder, assetRegistry));

		//assetImporters.push_back(std::shared_ptr<IAssetImporter>(new ModelImporter()));
		assetImporters.push_back(std::shared_ptr<SceneImporter>(new SceneImporter(assetRegistry)));
	}

	AssetManager::~AssetManager() {
		if(assetRegistry) {
			assetRegistry->OnAssetRegisteredEvent -= Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetRegisteredEvent);
			assetRegistry->OnAssetUnRegisteredEvent -= Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetUnRegisteredEvent);
			assetRegistry->OnAssetMovedOrRenamedEvent -= Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetMovedOrRenamedEvent);
		}
	}

	std::shared_ptr<AssetManager> AssetManager::Get() {
		return Engine::GetAssetManager();
	}

	void AssetManager::WriteDataToPath(const std::string& pathInProject, const std::shared_ptr<Asset>& asset, const bool writeNameValuePair) {
		WriteDataToFullPath(projectRoot + resourcesFolder + pathInProject, asset, writeNameValuePair);
	}

	void AssetManager::WriteDataToFullPath(const std::string& fullPath, const std::shared_ptr<Asset>& asset, const bool writeNameValuePair) {
		if (!asset->GetGuid().isValid()) {
			if (!assetRegistry->TryRegisterAsset(asset, fullPath)) {
				DEBUG_ERROR("Failed to process asset for: " + fullPath);
			} else {
			}
		}

		WriteDataToFullPath(fullPath, asset->GetName(), asset, asset->GetDefaultExtension(), writeNameValuePair);
	}

	bool AssetManager::FileExists(const std::string& fullPath, const std::string& fileName, const bool isRelativeFromProject) const {
		return FileExists(isRelativeFromProject ? projectRoot + resourcesFolder + fileName : fullPath + "/" + fileName, isRelativeFromProject);
	}

	bool AssetManager::FileExists(const std::string& fullPath, bool isRelativeFromProject) const {
		std::ifstream file(fullPath.c_str());

		const bool fileExists = file.is_open();
		file.close();

		return fileExists;
	}

	std::string AssetManager::GetDirectoryFromPath(const std::string& path) {
		const std::filesystem::path directoryPath { path };
		
		return directoryPath.parent_path().generic_string() + "/";
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

	std::vector<char> AssetManager::ReadFile(const std::string& fileName, const int fileOpenMode) const {
		std::ifstream file((projectRoot + resourcesFolder + fileName).c_str(), fileOpenMode);

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

	void AssetManager::HandleOnAssetRegisteredEvent(const std::shared_ptr<Asset>&, const std::string&) {
		SaveAssetRegistry();
	}

	void AssetManager::HandleOnAssetUnRegisteredEvent(const xg::Guid&, const std::string&) {
		SaveAssetRegistry();
	}

	void AssetManager::HandleOnAssetMovedOrRenamedEvent(const xg::Guid&, const std::string&, const std::string&, const std::string&, const std::string&) {
		SaveAssetRegistry();
	}

	void AssetManager::SaveAssetRegistry() {
		WriteDataToFullPath(projectRoot + nativeFolder, assetRegistryPath, assetRegistry);
	}

	// TODO: Replace with self managed update subscribe in FileWatcher itself
	void AssetManager::UpdateFileWatcher() const {
		fileWatcher->Update();
	}
}
