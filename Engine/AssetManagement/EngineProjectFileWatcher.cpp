#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/Utility/Logging.hpp"

#include <format>
#include <nameof.hpp>

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
			filename.c_str(),
			action,
			oldFilename.c_str()
		};

		assetActionsToProcess.emplace_back(assetAction);
	}

	void EngineProjectFileWatcher::Update() {
		if (assetActionsToProcess.empty()) return;

		for(const auto& [directory, fileName, action, oldFileName] : assetActionsToProcess) {
			const std::string pathInProject = AssetManager::FullPathToPathInProject(directory);
			const std::string fullPath = pathInProject + fileName;
			
			xg::Guid assetGuid;
			AssetRegistry::TryGetGuidForPath(pathInProject, fileName, assetGuid);

			switch (action) {
			case efsw::Action::Add:
			{
				// No need to process the asset again if it is already registered
				if (assetGuid.isValid()) continue;

				const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
				std::shared_ptr<IAssetImporter> assetImporter;
				if(!assetManager->TryGetAssetImporterForPath(fullPath, assetImporter)) { // TODO: Verify fullPath contains path before project root
					DEBUG_WARNING(std::format("No {} found for {}", NAMEOF_SHORT_TYPE(IAssetImporter).data(), fullPath));
					continue;
				}

				if (const std::shared_ptr<Asset>& processedAsset = assetImporter->ImportAsset(pathInProject, fileName); !processedAsset) {
					DEBUG_ERROR("Failed to process asset for: " + fullPath);
				} else {
					DEBUG_INFO("Successfully processed asset at path: " + fullPath);
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
				break;
			case efsw::Action::Moved:
			{
				const std::shared_ptr<AssetManager> assetManager = AssetManager::Get();
				if (assetGuid.isValid())
					assetRegistry->TryUpdatePathForGuid(assetGuid, pathInProject, fileName);
				// No need to log an error here, the user could have renamed an asset that wasn't even registered to begin with
				// Warning them about that only provides clutter in the logs
			}
			break;
			}
		}

		assetActionsToProcess.clear();
	}
}
