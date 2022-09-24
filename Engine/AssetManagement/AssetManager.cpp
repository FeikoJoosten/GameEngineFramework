#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Engine/Engine.hpp"

#include <filesystem>

namespace Engine {

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
		return fullExecutableDirectory;
	}

	std::vector<char> AssetManager::ReadFile(const std::string& fileName, const int fileOpenMode) const {
		std::ifstream file((fullExecutableDirectory + fileName).c_str(), fileOpenMode);

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

	void AssetManager::SetExecutablePath(const std::string& executablePath) {
		std::filesystem::path path(executablePath);
		fullExecutablePath = path.generic_string();
		fullExecutableDirectory = path.remove_filename().generic_string();
		std::cout << fullExecutableDirectory << std::endl;
	}
}
