#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Utility/Defines.hpp"

#include <efsw/efsw.hpp>
#include <memory>

namespace Engine {
	class AssetRegistry;

	class ENGINE_LOCAL EngineProjectFileWatcher : public efsw::FileWatchListener {
		friend class AssetManager;

		std::unique_ptr<efsw::FileWatcher> fileWatcher {};
		std::shared_ptr<AssetRegistry> assetRegistry {};
		efsw::WatchID ownWatchId {};

		EngineProjectFileWatcher(const std::string& directoryToWatch, std::shared_ptr<AssetRegistry> assetRegistry);

	public:
		EngineProjectFileWatcher() = delete;
		virtual ~EngineProjectFileWatcher() override;
		EngineProjectFileWatcher(const EngineProjectFileWatcher& other) = delete;
		EngineProjectFileWatcher(EngineProjectFileWatcher&& other) = delete;
		EngineProjectFileWatcher& operator=(const EngineProjectFileWatcher& other) = delete;
		EngineProjectFileWatcher& operator=(EngineProjectFileWatcher&& other) = delete;

		virtual void handleFileAction(efsw::WatchID watchId, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) override;
	};
}
