#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Engine.hpp"

#include <filesystem>

namespace Engine {

	std::shared_ptr<AssetManager> AssetManager::Get() {
		return Engine::GetAssetManager();
	}

	const std::string& AssetManager::GetProjectRoot() {
		return fullExecutableDirectory;
	}

	void AssetManager::SetExecutablePath(const std::string& executablePath) {
		std::filesystem::path path(executablePath);
		fullExecutablePath = path.generic_string();
		fullExecutableDirectory = path.remove_filename().generic_string();
		std::cout << fullExecutableDirectory << std::endl;
	}
}
