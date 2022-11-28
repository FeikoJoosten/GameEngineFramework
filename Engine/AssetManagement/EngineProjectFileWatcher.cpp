#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/Utility/Logging.hpp"

#include <filesystem>

namespace Engine {

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
		AssetAction assetAction = {
			dir + "/",
			filename,
			action,
			oldFilename
		};

		assetActionsToProcess.emplace_back(assetAction);
	}

	void EngineProjectFileWatcher::Update() {
		if (assetActionsToProcess.empty()) return;

		for(const AssetAction& assetAction : assetActionsToProcess) {
			const std::filesystem::path path = assetAction.directory + assetAction.fileName;
			const std::string fullPath = path.generic_string();

			switch (assetAction.action) {
			case efsw::Action::Add:
			{
				bool processed = false;
				const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
				const std::vector<std::shared_ptr<IAssetImporter>>& assetImporters = assetManager->GetAllAssetImporters();
				for (const std::shared_ptr<IAssetImporter>& assetImporter : assetImporters) {
					if (!assetImporter->SupportsFileExtension(path.extension().generic_string())) continue;

					const std::shared_ptr<Asset> processedAsset = assetImporter->ProcessAsset(assetAction.directory, assetAction.fileName);
					processed = true;
					if (!processedAsset) {
						DEBUG_ERROR("Failed to process asset for: " + fullPath);
					} else {
						DEBUG_INFO("Successfully processed asset at path: " + fullPath);
					}
				}

				if (!processed) DEBUG_WARNING("No AssetImporter found to support asset at path: " + fullPath);
			}
			break;
			case efsw::Action::Delete:
			{
				xg::Guid assetGuid {};
				if (assetRegistry->TryGetGuidForPath(assetAction.directory, assetAction.fileName, assetGuid))
					assetRegistry->TryUnRegisterAsset(assetGuid);
			}
			break;
			case efsw::Action::Modified:
				break;
			case efsw::Action::Moved:
			{
				xg::Guid assetGuid {};
				if (assetRegistry->TryGetGuidForPath(assetAction.directory, assetAction.oldFileName, assetGuid))
					assetRegistry->TryUpdatePathForGuid(assetGuid, fullPath, assetAction.fileName);
				// No need to log an error here, could the user could have renamed an asset that wasn't even registered to begin with
				// Warning them about that only provides clutter in the logs
			}
			break;
			}
		}

		assetActionsToProcess.clear();
	}
}
