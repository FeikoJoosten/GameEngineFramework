#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Utility/Logging.hpp"

#include <fstream>
#include <memory>
#include <string>
#include <filesystem>
#include <cereal/archives/json.hpp>
#include <sago/PlatformFolders.hpp>

namespace Engine {
	class ENGINE_LOCAL EngineAssetManager {
		friend std::shared_ptr<EngineAssetManager> Engine::GetEngineAssetManager() noexcept;

		EngineAssetManager() = default;

	public:
		EngineAssetManager(const EngineAssetManager& other) = delete;
		EngineAssetManager(EngineAssetManager&& other) noexcept = delete;
		~EngineAssetManager() = default;

		EngineAssetManager& operator=(const EngineAssetManager& other) = delete;
		EngineAssetManager& operator=(EngineAssetManager&& other) noexcept = delete;

		static std::shared_ptr<EngineAssetManager> Get();

		inline static const std::string ENGINE_SETTINGS_FILE_TYPE = ".json";

		template<typename T>
		static void WriteDataToPath(const std::string& relativePath, T data, bool writeNameValuePairs = true);

		template<typename T>
		static T ReadDataFromPath(const std::string& relativePath);

	private:
		inline static const std::string SYSTEM_CONFIG_FOLDER = sago::getConfigHome() + "/EngineFramework/";
	};

	template <typename T> void EngineAssetManager::WriteDataToPath(const std::string& relativePath, T data, bool writeNameValuePairs) {
		const std::string fullPath = SYSTEM_CONFIG_FOLDER + relativePath;
		if (const std::string desiredDirectoryPath = AssetManager::GetDirectoryFromPath(fullPath);
			!std::filesystem::is_directory(desiredDirectoryPath)) {
			if (!std::filesystem::create_directories(desiredDirectoryPath)) {
				DEBUG_ERROR("Failed to create directories for path: " + relativePath);
				return;
			}
		}
		
		std::ofstream outputStream(fullPath);
		cereal::JSONOutputArchive archive(outputStream);
		
		writeNameValuePairs ? archive(CEREAL_NVP(data)) : archive(data);
	}

	template <typename T> T EngineAssetManager::ReadDataFromPath(const std::string& relativePath) {
		const std::string fullPath = SYSTEM_CONFIG_FOLDER + relativePath;
		if(AssetManager::FileExists(fullPath, true)) {
			if (std::ifstream inputStream(fullPath); inputStream.good()) {
				cereal::JSONInputArchive archive(inputStream);
				T output {};
				archive(output);
				return output;
			}
		}
		return {};
	}
}
