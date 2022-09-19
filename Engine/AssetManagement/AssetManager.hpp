#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Utility/Utility.hpp"
#include "Engine/Utility/Logging.hpp"

#include <fstream>
#include <memory>
#include <string>
#include <filesystem>
#include <cereal/archives/json.hpp>

namespace Engine {
	class ENGINE_API AssetManager {
		friend std::shared_ptr<AssetManager> Engine::GetAssetManager() noexcept;

		AssetManager() = default;

	public:
		AssetManager(const AssetManager& other) = delete;
		AssetManager(AssetManager&& other) noexcept = delete;
		~AssetManager() = default;

		AssetManager& operator=(const AssetManager& other) = delete;
		AssetManager& operator=(AssetManager&& other) noexcept = delete;

		static std::shared_ptr<AssetManager> Get();

		const std::string& GetProjectRoot();

		static std::string GetDirectoryFromPath(const std::string& path);

		template<typename T>
		static void WriteDataToPath(const std::string& fullPath, T data, bool writeNameValuePairs = true);

		template<typename T>
		static T ReadDataFromPath(const std::string& fullPath);

		// TODO: Make this private
		void SetExecutablePath(const std::string& executablePath);

	private:
		std::string fullExecutablePath {};
		std::string fullExecutableDirectory {};
	};

	template <typename T> void AssetManager::WriteDataToPath(const std::string& fullPath, T data, bool writeNameValuePairs) {
		if (const std::string desiredDirectoryPath = GetDirectoryFromPath(fullPath); !std::filesystem::is_directory(desiredDirectoryPath)) {
			if (!std::filesystem::create_directories(desiredDirectoryPath)) {
				DEBUG_ERROR("Failed to create directories for path: " + fullPath);
				return;
			}
		}

		std::ofstream outputStream(fullPath);
		cereal::JSONOutputArchive archive(outputStream);

		writeNameValuePairs ? archive(CEREAL_NVP(data)) : archive(data);
	}

	template <typename T> T AssetManager::ReadDataFromPath(const std::string& fullPath) {
		if(Utility::FileExists(fullPath, true)) {
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
