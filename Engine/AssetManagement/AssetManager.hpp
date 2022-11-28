#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/AssetManagement/IAssetImporter.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Utility/Logging.hpp"

#include <fstream>
#include <filesystem>

namespace Engine {
	class ENGINE_API AssetManager {
		friend class Engine;
		friend void Application::Update();

		static inline std::string projectRoot {}; // Assigned by Engine::Engine()
		static inline std::string resourcesFolder = "Resources/";
		static inline std::string nativeFolder = "Native/";
		std::vector<std::shared_ptr<IAssetImporter>> assetImporters {};

		std::shared_ptr<AssetRegistry> assetRegistry;
		std::unique_ptr<EngineProjectFileWatcher> fileWatcher {};

		AssetManager();

	public:
		~AssetManager();
		AssetManager(const AssetManager& other) = delete;
		AssetManager(AssetManager&& other) noexcept = delete;

		AssetManager& operator=(const AssetManager& other) = delete;
		AssetManager& operator=(AssetManager&& other) noexcept = delete;

		static std::shared_ptr<AssetManager> Get();

		void WriteDataToPath(const std::string& pathInProject, const std::shared_ptr<Asset>& asset, bool writeNameValuePair = true);

		template<class T>
		void WriteDataToPath(const std::string& pathInProject, const std::string& fileName, T data, const std::string& fileExtension = "", bool writeNameValuePairs = true);

		void WriteDataToFullPath(const std::string& fullPath, const std::shared_ptr<Asset>& asset, bool writeNameValuePair = true);

		template<class T>
		void WriteDataToFullPath(const std::string& fullPath, const std::string& fileName, T data, const std::string& fileExtension = "", bool writeNameValuePairs = true);

		template<typename T>
		[[nodiscard]] T ReadDataFromPath(const std::string& pathInProject, const std::string& fileName);

		template<typename T>
		[[nodiscard]] T ReadDataFromFullPath(const std::string& fullPath, const std::string& fileName);

		[[nodiscard]] bool FileExists(const std::string& fullPath, const std::string& fileName, bool isRelativeFromProject = false) const;

		[[nodiscard]] bool FileExists(const std::string& fullPath, bool isRelativeFromProject = false) const;

		static std::string GetDirectoryFromPath(const std::string& path);

		[[nodiscard]] static const std::string& GetProjectRoot();

		[[nodiscard]] std::shared_ptr<AssetRegistry> GetAssetRegistry();

		[[nodiscard]] std::vector<std::shared_ptr<IAssetImporter>> GetAllAssetImporters() const;

		template<class T>
		[[nodiscard]] std::shared_ptr<T> GetAssetImporter();

		[[nodiscard]] std::vector<char> ReadFile(const std::string& fileName, int fileOpenMode = std::ios_base::in) const;

	private:
		void HandleOnAssetRegisteredEvent(const std::shared_ptr<Asset>& registeredAsset, const std::string& assetPath);

		void HandleOnAssetUnRegisteredEvent(const xg::Guid& unregisteredAssetGuid, const std::string& unregisteredAssetPath);

		void HandleOnAssetMovedOrRenamedEvent(const xg::Guid& assetGuid, const std::string& oldPathInProject, const std::string& newPathInProject, const std::string& oldAssetName, const std::string& newAssetName);

		void SaveAssetRegistry();

		void UpdateFileWatcher() const;
	};

	template <typename T>
	void AssetManager::WriteDataToPath(const std::string& pathInProject, const std::string& fileName, T data, const std::string& fileExtension, const bool writeNameValuePairs) {
		WriteDataToFullPath(projectRoot + resourcesFolder + pathInProject, fileName, data, fileExtension, writeNameValuePairs);
	}

	template <class T>
	void AssetManager::WriteDataToFullPath(const std::string& fullPath, const std::string& fileName, T data, const std::string& fileExtension, const bool writeNameValuePairs) {
		const std::string desiredDirectoryPath = GetDirectoryFromPath(fullPath);
		if (!std::filesystem::is_directory(desiredDirectoryPath)) {
			if (!std::filesystem::create_directories(desiredDirectoryPath)) {
				DEBUG_ERROR("Failed to create directories for path: " + fullPath);
				return;
			}
		}

		std::ofstream outputStream(desiredDirectoryPath + fileName + fileExtension);
		cereal::JSONOutputArchive archive(outputStream);

		writeNameValuePairs ? archive(CEREAL_NVP(data)) : archive(data);
	}

	template <typename T>
	T AssetManager::ReadDataFromPath(const std::string& pathInProject, const std::string& fileName) {
		return ReadDataFromFullPath<T>(projectRoot + resourcesFolder + pathInProject, fileName);
	}

	template <typename T>
	T AssetManager::ReadDataFromFullPath(const std::string& fullPath, const std::string& fileName) {
		const std::string absolutePath = fullPath + fileName;
		
		if (FileExists(absolutePath)) {
			if (std::ifstream inputStream(absolutePath); inputStream.good()) {
				cereal::JSONInputArchive archive(inputStream);
				T output;
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
			const type_info& importerType = typeid(*assetImporter);
			if (assetImporterType != importerType) continue;

			return std::static_pointer_cast<T>(assetImporter);
		}

		return {};
	}
}
