#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Utility/Logging.hpp"

#include <fstream>
#include <memory>
#include <string>
#include <filesystem>
#include <cereal/archives/json.hpp>
#include <sago/PlatformFolders.hpp>
#include <nameof.hpp>

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

		template<class OwnerType, class DataType>
		static void SaveEngineData(const DataType& input, bool writeNameValuePairs = true, const std::string& fileExtension = ENGINE_SETTINGS_FILE_TYPE);

		template<class OwnerType, class DataType>
		static bool TryLoadEngineData(DataType& output, const std::string& fileExtension = ENGINE_SETTINGS_FILE_TYPE);

	private:
		inline static const std::string SYSTEM_CONFIG_FOLDER = sago::getConfigHome() + "/EngineFramework/";

		template<class OwnerType, class DataType>
		[[nodiscard]] static std::string MakeEngineFilePath(const std::string& fileExtension);
	};

	template <class OwnerType, class DataType>
	void EngineAssetManager::SaveEngineData(const DataType& input, bool writeNameValuePairs, const std::string& fileExtension) {
		const std::string filePath = MakeEngineFilePath<OwnerType, DataType>(fileExtension);
		if(const std::string desiredDirectoryPath = AssetManager::GetDirectoryFromPath(filePath); !std::filesystem::is_directory(desiredDirectoryPath)) {
			if(!std::filesystem::create_directories(desiredDirectoryPath) && !std::filesystem::exists(desiredDirectoryPath)) {
				DEBUG_ERROR("Failed to create directories for path: " + filePath);
				return;
			}
		}

		std::ofstream outputStream(filePath);
		cereal::JSONOutputArchive archive(outputStream);

		writeNameValuePairs ? archive(cereal::make_nvp(static_cast<std::string>(NAMEOF_SHORT_TYPE(OwnerType)), input)) : archive(input);
	}

	template <class OwnerType, class DataType>
	bool EngineAssetManager::TryLoadEngineData(DataType& output,	const std::string& fileExtension) {
		const std::string filePath = MakeEngineFilePath<OwnerType, DataType>(fileExtension);
		if(std::ifstream inputStream(filePath); inputStream.good() && inputStream.is_open()) {
			cereal::JSONInputArchive archive(inputStream);
			output = {};
			archive(output);
			return true;
		}
		return false;
	}

	template <class OwnerType, class DataType>
	std::string EngineAssetManager::MakeEngineFilePath(const std::string& fileExtension) {
		return std::move(SYSTEM_CONFIG_FOLDER + static_cast<std::string>(NAMEOF_SHORT_TYPE(OwnerType)) + "/" + static_cast<std::string>(NAMEOF_SHORT_TYPE(DataType)) + fileExtension);
	}
}
