#pragma once

#include "Engine/Api/Api.hpp"

#include <efsw/efsw.hpp>
#include <memory>
#include <vector>

namespace Engine {
	class AssetRegistry;

	struct ENGINE_LOCAL AssetAction {
		const std::string directory;
		const std::string fileName;
		const efsw::Action action;
		const std::string oldFileName;

		bool operator==(const AssetAction& other) const;
	};

	class ENGINE_LOCAL EngineProjectFileWatcher : public efsw::FileWatchListener {
		friend class AssetManager;

		std::unique_ptr<efsw::FileWatcher> fileWatcher {};
		std::shared_ptr<AssetRegistry> assetRegistry {};
		efsw::WatchID ownWatchId {};
		std::vector<AssetAction> assetActionsToProcess {};

		EngineProjectFileWatcher(const std::string& directoryToWatch, std::shared_ptr<AssetRegistry> assetRegistry);

	public:
		EngineProjectFileWatcher() = delete;
		virtual ~EngineProjectFileWatcher() override;
		EngineProjectFileWatcher(const EngineProjectFileWatcher& other) = delete;
		EngineProjectFileWatcher(EngineProjectFileWatcher&& other) = delete;
		EngineProjectFileWatcher& operator=(const EngineProjectFileWatcher& other) = delete;
		EngineProjectFileWatcher& operator=(EngineProjectFileWatcher&& other) = delete;

	private:
		virtual void handleFileAction(efsw::WatchID watchId, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) override;

		void Update();
	};
}
