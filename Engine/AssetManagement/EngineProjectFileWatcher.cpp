#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/Utility/Logging.hpp"

#include <filesystem>

namespace Engine {
	EngineProjectFileWatcher::EngineProjectFileWatcher(const std::string& directoryToWatch, std::shared_ptr<AssetRegistry> assetRegistry) : assetRegistry(assetRegistry) {
		fileWatcher = std::make_unique<efsw::FileWatcher>();

		ownWatchId = fileWatcher->addWatch(directoryToWatch, this, true);
		fileWatcher->watch();
	}

	EngineProjectFileWatcher::~EngineProjectFileWatcher() {
		fileWatcher->removeWatch(ownWatchId);
	}

	void EngineProjectFileWatcher::handleFileAction(efsw::WatchID watchId, const std::string& dir, const std::string& filename, const efsw::Action action, const std::string oldFilename) {
		const std::filesystem::path path = dir + "/" + filename;
		const std::string fullPath = path.generic_string();
		
		switch (action) {
		case efsw::Action::Add:	{
			bool processed = false;
			const std::vector<std::shared_ptr<IAssetImporter>>& assetImporters = AssetManager::Get()->GetAllAssetImporters();
			for (const std::shared_ptr<IAssetImporter>& assetImporter : assetImporters) {
				if (!assetImporter->SupportsFileExtension(path.extension().generic_string().c_str())) continue;

				std::shared_ptr<Asset> processedAsset = assetImporter->ProcessAsset(fullPath.c_str());
				processed = true;
				if (!processedAsset || !assetRegistry->TryRegisterAsset(processedAsset, fullPath)) {
					DEBUG_ERROR("Failed to process asset for: " + fullPath);
				} else {
					DEBUG_INFO("Successfully processed asset at path: " + fullPath);
				}
			}

			if(!processed) {
				DEBUG_WARNING("No AssetImporter found to support asset at path: " + fullPath);
			}
		}
			break;
		case efsw::Action::Delete: {
			xg::Guid assetGuid {};
			if (assetRegistry->TryGetGuidForPath(fullPath, assetGuid))
				assetRegistry->TryUnRegisterAsset(assetGuid);
		}
			break;
		case efsw::Action::Modified:
			break;
		case efsw::Action::Moved: {
			xg::Guid assetGuid {};
			if (assetRegistry->TryGetGuidForPath(dir + "/" + oldFilename, assetGuid))
				assetRegistry->TryUpdatePathForGuid(assetGuid, fullPath);
			// No need to log an error here, could the user could have renamed an asset that wasn't even registered to begin with
			// Warning them about that only provides clutter in the logs
		}
			break;
		}
	}
}
