#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/AssetManagement/ModelImporter.hpp"
#include "Engine/Engine/Engine.hpp"

#include <nameof.hpp>
#include <filesystem>

namespace Engine {
	const std::string assetRegistryPath = static_cast<std::string>(NAMEOF_SHORT_TYPE(AssetRegistry)) + "/" + static_cast<std::string>(NAMEOF(assetRegistryPath));

	AssetManager::AssetManager(std::string projectRoot) : projectRoot(std::move(projectRoot)) {
		assetImporters.push_back(std::shared_ptr<IAssetImporter>(new ModelImporter()));
		
		assetRegistry = ReadDataFromPath<std::shared_ptr<AssetRegistry>>(this->projectRoot + assetRegistryPath);
		if (!assetRegistry) assetRegistry = std::shared_ptr<AssetRegistry>(new AssetRegistry());

		assetRegistry->OnAssetRegisteredEvent += Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetRegisteredEvent);
		assetRegistry->OnAssetUnRegisteredEvent += Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetUnRegisteredEvent);
		assetRegistry->OnAssetMovedOrRenamedEvent += Sharp::EventHandler::Bind(this, &AssetManager::HandleOnAssetMovedOrRenamedEvent);

		fileWatcher = std::unique_ptr<EngineProjectFileWatcher>(new EngineProjectFileWatcher(this->projectRoot + "Resources", assetRegistry));
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

	bool AssetManager::FileExists(const std::string& fileName, const bool isFullPath) {
		std::ifstream file((isFullPath ? fileName : Get()->GetProjectRoot() + fileName).c_str());

		const bool fileExists = file.is_open();
		file.close();

		return fileExists;
	}

	std::string AssetManager::GetDirectoryFromPath(const std::string& path) {
		const size_t pos = path.find_last_of("\\/");
		return (std::string::npos == pos)
			? std::string()
			: path.substr(0, pos);
	}

	const std::string& AssetManager::GetProjectRoot() {
		return projectRoot;
	}

	std::vector<std::shared_ptr<IAssetImporter>> AssetManager::GetAllAssetImporters() const {
		return assetImporters;
	}

	std::vector<char> AssetManager::ReadFile(const std::string& fileName, const int fileOpenMode) const {
		std::ifstream file((projectRoot + fileName).c_str(), fileOpenMode);

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

	void AssetManager::HandleOnAssetRegisteredEvent(const std::shared_ptr<Asset>& registeredAsset, const std::string& assetPath) {
		SaveAssetRegistry();
	}

	void AssetManager::HandleOnAssetUnRegisteredEvent(const xg::Guid& unregisteredAssetGuid, const std::string& unregisteredAssetPath) {
		SaveAssetRegistry();
	}

	void AssetManager::HandleOnAssetMovedOrRenamedEvent(const xg::Guid& assetGuid, const std::string& oldAssetPath, const std::string& newAssetPath) {
		SaveAssetRegistry();
	}

	void AssetManager::SaveAssetRegistry() const {
		WriteDataToPath(projectRoot + assetRegistryPath, assetRegistry);
	}
}
