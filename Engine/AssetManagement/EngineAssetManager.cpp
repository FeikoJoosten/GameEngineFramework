#include "Engine/AssetManagement/EngineAssetManager.hpp"
#include "Engine/Engine.hpp"

namespace Engine {
	std::shared_ptr<EngineAssetManager> EngineAssetManager::Get() {
		return Engine::GetEngineAssetManager();
	}
}
