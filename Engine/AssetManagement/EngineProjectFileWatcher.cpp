#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/Utility/Logging.hpp"

#include <format>
#include <nameof.hpp>

namespace Engine {

	bool AssetAction::operator==(const AssetAction& other) const {
		if (other.action != action) return false;
		if (other.fileName != fileName) return false;
		if (other.oldFileName != oldFileName) return false;
		if (other.directory != directory) return false;

		return true;
	}

	EngineProjectFileWatcher::EngineProjectFileWatcher(const std::string& directoryToWatch, std::shared_ptr<AssetRegistry> assetRegistry) : assetRegistry(std::move(assetRegistry)) {
		fileWatcher = std::make_unique<efsw::FileWatcher>();

		ownWatchId = fileWatcher->addWatch(directoryToWatch, this, true);
		fileWatcher->watch();
	}

	EngineProjectFileWatcher::~EngineProjectFileWatcher() {
		fileWatcher->removeWatch(ownWatchId);
	}

	void EngineProjectFileWatcher::handleFileAction(efsw::WatchID watchId, const std::string& dir, const std::string& filename, const efsw::Action action, const std::string oldFilename) {
		// Delaying the processing of the asset update ensures the file is fully written
		// Also make copies of the strings to prevent memory being lost
		AssetAction assetAction = {
			dir + "/",
			filename,
			action,
			oldFilename
		};

		if(std::ranges::find(assetActionsToProcess, assetAction) == assetActionsToProcess.end())
			assetActionsToProcess.push_back(std::move(assetAction));
	}

	void EngineProjectFileWatcher::Update() {
		if (assetActionsToProcess.empty()) return;

		for(const AssetAction& assetAction : assetActionsToProcess) {
			const std::string fullPath = assetAction.directory;
			const std::string pathInProject = AssetManager::FullPathToPathInProject(assetAction.directory);
			const char* fileNameAsConstChar = assetAction.fileName.c_str();
			const std::string fullPathWithAssetName = pathInProject + assetAction.fileName;
			
			xg::Guid assetGuid;
			AssetRegistry::TryGetGuidForPath(pathInProject, fileNameAsConstChar, assetGuid);

			switch (assetAction.action) {
			case efsw::Action::Add:
			{
				// No need to process the asset again if it is already registered
				if (assetGuid.isValid()) continue;

				const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
				std::shared_ptr<IAssetImporter> assetImporter;
				if(!assetManager->TryGetAssetImporterForPath(fullPathWithAssetName, assetImporter)) { // TODO: Verify fullPath contains path before project root
					DEBUG_WARNING(std::format("No {} found for {}", NAMEOF_SHORT_TYPE(IAssetImporter).data(), fullPathWithAssetName));
					continue;
				}

				if (const std::shared_ptr<Asset>& processedAsset = assetImporter->ImportAsset(pathInProject, fileNameAsConstChar); !processedAsset) {
					DEBUG_ERROR("Failed to process asset for: " + fullPathWithAssetName);
				} else {
					DEBUG_INFO("Successfully processed asset at path: " + fullPathWithAssetName);
				}
			}
			break;
			case efsw::Action::Delete:
			{
				if (assetGuid.isValid())
					assetRegistry->TryUnRegisterAsset(assetGuid);
			}
			break;
			case efsw::Action::Modified:
				// TODO: Reimport asset
				break;
			case efsw::Action::Moved:
			{
				const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
				if (assetGuid.isValid())
					assetRegistry->TryUpdatePathForGuid(assetGuid, fullPath, fileNameAsConstChar);
				// No need to log an error here, the user could have renamed an asset that wasn't even registered to begin with
				// Warning them about that only provides clutter in the logs
			}
			break;
			}
		}

		assetActionsToProcess.clear();
	}
}
