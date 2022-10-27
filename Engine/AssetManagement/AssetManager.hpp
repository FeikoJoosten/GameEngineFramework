#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetImporter.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Utility/Logging.hpp"

#include <fstream>
#include <filesystem>

#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/json.hpp>

namespace Engine {
	class ENGINE_API AssetManager {
		friend std::shared_ptr<AssetManager> Engine::GetAssetManager() noexcept;

		explicit AssetManager(std::string projectRoot);

	public:
		AssetManager(const AssetManager& other) = delete;
		AssetManager(AssetManager&& other) noexcept = delete;
		~AssetManager() = default;

		AssetManager& operator=(const AssetManager& other) = delete;
		AssetManager& operator=(AssetManager&& other) noexcept = delete;

		static std::shared_ptr<AssetManager> Get();

		template<class T>
		static void WriteDataToPath(const std::string& fullPath, T data, bool writeNameValuePairs = true);

		template<class T>
		static T ReadDataFromPath(const std::string& fullPath);

		static bool FileExists(const std::string& fileName, bool isFullPath = false);

		static std::string GetDirectoryFromPath(const std::string& path);

		const std::string& GetProjectRoot();

		template<class T>
		[[nodiscard]] std::shared_ptr<T> GetAssetImporter();

		[[nodiscard]] std::vector<char> ReadFile(const std::string& fileName, int fileOpenMode = std::ios_base::in) const;

	private:
		std::string projectRoot {};
		std::vector<std::shared_ptr<IAssetImporter>> assetImporters {};
	};

	template <typename T>
	void AssetManager::WriteDataToPath(const std::string& fullPath, T data, bool writeNameValuePairs) {
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

	template <typename T> T
	AssetManager::ReadDataFromPath(const std::string& fullPath) {
		if(FileExists(fullPath, true)) {
			if (std::ifstream inputStream(fullPath); inputStream.good()) {
				cereal::JSONInputArchive archive(inputStream);
				T output {};
				archive(output);
				return output;
			}
		}
		return {};
	}

	template <class T>
	std::shared_ptr<T> AssetManager::GetAssetImporter() {
		const type_info& assetImporterType = typeid(T);
		for(const std::shared_ptr<IAssetImporter>& assetImporter : assetImporters) {
			if (assetImporterType != typeid(assetImporter.get())) continue;

			return std::static_pointer_cast<T>(assetImporter);
		}

		return {};
	}
}
