#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/AssetSerializationType.hpp"
#include "Engine/AssetManagement/IAssetImporter.hpp"
#include "Engine/AssetManagement/EngineProjectFileWatcher.hpp"
#include "Engine/Engine/Engine.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Logging.hpp"
#include "Engine/Utility/StringUtility.hpp"

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
		~AssetManager() = default;
		AssetManager(const AssetManager& other) = delete;
		AssetManager(AssetManager&& other) noexcept = delete;

		AssetManager& operator=(const AssetManager& other) = delete;
		AssetManager& operator=(AssetManager&& other) noexcept = delete;

		static std::shared_ptr<AssetManager> Get();

		void WriteAssetToPath(const std::string& pathInProject, const std::shared_ptr<Asset>& asset, const char* assetExtension = nullptr, const AssetSerializationType& serializationType = Json, bool writeNameValuePair = true);

		template<class T>
		void WriteDataToPath(const std::string& pathInProject, const std::string& fileName, T data, const std::string& fileExtension = "", const AssetSerializationType& serializationType = Json, bool writeNameValuePairs = true);

		void WriteAssetToFullPath(const std::string& fullPath, const std::shared_ptr<Asset>& asset, const char* assetExtension = nullptr, const AssetSerializationType& serializationType = Json, bool writeNameValuePair = true);

		template<class T>
		void WriteDataToFullPath(const std::string& fullPath, const std::string& fileName, T data, const std::string& fileExtension = "", const AssetSerializationType& serializationType = Json, bool writeNameValuePairs = true);

		template<typename T>
		[[nodiscard]] std::shared_ptr<T> LoadAsset(const xg::Guid& assetGuid);

		template<typename T>
		[[nodiscard]] static T ReadDataFromPath(const std::string& pathInProject, const std::string& fileName, const AssetSerializationType& serializationType = Json);

		template<typename T>
		[[nodiscard]] static T ReadDataFromFullPath(const std::string& fullPath, const std::string& fileName, const AssetSerializationType& serializationType = Json);

		[[nodiscard]] static bool FileExists(const std::string& fullPath, const std::string& fileName, bool isRelativeFromProject = false);

		[[nodiscard]] static bool FileExists(const std::string& fullPath);

		static std::string GetDirectoryFromPath(const std::string& path);

		[[nodiscard]] static const std::string& GetProjectRoot();

		[[nodiscard]] std::shared_ptr<AssetRegistry> GetAssetRegistry();

		[[nodiscard]] std::vector<std::shared_ptr<IAssetImporter>> GetAllAssetImporters() const;

		template<class T>
		[[nodiscard]] std::shared_ptr<T> GetAssetImporter();

		template<class T>
		[[nodiscard]] std::shared_ptr<IAssetImporter> GetAssetImporterForAssetType() const;

		[[nodiscard]] bool TryGetAssetImporterForPath(const std::string& pathInProject, std::shared_ptr<IAssetImporter>& outputAssetImporter) const;

		[[nodiscard]] static std::vector<char> ReadRawFile(const std::string& fileName, int fileOpenMode = std::ios_base::in);

		void DeleteFile(const std::string& fullPath, const std::string& fileName, bool isRelativeFromProject = false) const;

		[[nodiscard]] static std::string PathInProjectToFullPath(const std::string& pathInProject);

		[[nodiscard]] static std::string FullPathToPathInProject(const std::string& fullPath);

		ENGINE_LOCAL [[nodiscard]] static std::string& GetNativeFolder();

	private:

		void UpdateFileWatcher() const;
	};

	template <typename T>
	void AssetManager::WriteDataToPath(const std::string& pathInProject, const std::string& fileName, T data, const std::string& fileExtension, const AssetSerializationType& serializationType, const bool writeNameValuePairs) {
		WriteDataToFullPath(projectRoot + resourcesFolder + pathInProject, fileName, data, fileExtension, serializationType, writeNameValuePairs);
	}

	template <class T>
	void AssetManager::WriteDataToFullPath(const std::string& fullPath, const std::string& fileName, T data, const std::string& fileExtension, const AssetSerializationType& serializationType, const bool writeNameValuePairs) {
		const std::string desiredDirectoryPath = GetDirectoryFromPath(fullPath);
		if (!std::filesystem::is_directory(desiredDirectoryPath)) {
			std::string outputDirectory = desiredDirectoryPath;
			if (outputDirectory[outputDirectory.length() - 1] == '/')
				outputDirectory = outputDirectory.substr(0, outputDirectory.length() - 1);
			if (!std::filesystem::create_directories(outputDirectory) && !std::filesystem::exists(desiredDirectoryPath)) {
				DEBUG_ERROR("Failed to create directories for path: " + fullPath);
				return;
			}
		}

		std::ofstream outputStream(desiredDirectoryPath + fileName + fileExtension);

		switch (serializationType) {
		case Json:
		{
			cereal::JSONOutputArchive jsonArchive(outputStream);
			writeNameValuePairs ? jsonArchive(CEREAL_NVP(data)) : jsonArchive(data);
			break;
		}
		case Binary:
		{
			cereal::BinaryOutputArchive binaryArchive(outputStream);
			binaryArchive(data);
			break;
		}
		case Xml:
		{
			cereal::XMLOutputArchive xmlArchive(outputStream);
			writeNameValuePairs ? xmlArchive(CEREAL_NVP(data)) : xmlArchive(data);
			break;
		}
		default: throw std::runtime_error("Invalid AssetSerializationType provided!");
		}

		outputStream.close();
	}

	template <typename T>
	std::shared_ptr<T> AssetManager::LoadAsset(const xg::Guid& assetGuid) {
		const std::shared_ptr<IAssetImporter> assetImporter = GetAssetImporterForAssetType<T>();
		if (!assetImporter) return {};

		if (const std::shared_ptr<Asset> loadedAsset = assetImporter->LoadAsset(assetGuid)) return std::static_pointer_cast<T>(loadedAsset);

		return {};
	}

	template <typename T>
	T AssetManager::ReadDataFromPath(const std::string& pathInProject, const std::string& fileName, const AssetSerializationType& serializationType) {
		return ReadDataFromFullPath<T>(projectRoot + resourcesFolder + pathInProject, fileName, serializationType);
	}

	template <typename T>
	T AssetManager::ReadDataFromFullPath(const std::string& fullPath, const std::string& fileName, const AssetSerializationType& serializationType) {
		const std::string absolutePath = StringUtility::SanitizePath(fullPath) + fileName;
		std::ifstream inputStream(absolutePath);
		if (!inputStream.good() || !inputStream.is_open()) return {};

		T output = {};

		// A try catch is required in case the deserialization fails which can occur if the serialization layout has been changed without updating the serialized data
		try {
			switch (serializationType) {
			case Json:
			{
				cereal::JSONInputArchive jsonArchive(inputStream);
				jsonArchive(output);
				break;
			}
			case Binary:
			{
				cereal::BinaryInputArchive binaryArchive(inputStream);
				binaryArchive(output);
				break;
			}
			case Xml:
			{
				cereal::XMLInputArchive xmlArchive(inputStream);
				xmlArchive(output);
				break;
			}
			default: throw std::runtime_error("Invalid AssetSerializationType provided!");
			}
		}
		catch (...) { }

		inputStream.close();
		return output;
	}

	template <class T>
	std::shared_ptr<T> AssetManager::GetAssetImporter() {
		static_assert(std::is_base_of_v<IAssetImporter, T>, "T must derive from IAssetImporter");

		for (const std::shared_ptr<IAssetImporter>& assetImporter : assetImporters)
			if (std::shared_ptr<T> castAssetImporter = std::dynamic_pointer_cast<T>(assetImporter)) return castAssetImporter;

		return {};
	}

	template <class T>
	std::shared_ptr<IAssetImporter> AssetManager::GetAssetImporterForAssetType() const {
		static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

		const type_info& assetType = typeid(T);
		for (const std::shared_ptr<IAssetImporter>& assetImporter : assetImporters) {
			if (!assetImporter->SupportsType(assetType)) continue;

			return assetImporter;
		}

		return {};
	}
}
