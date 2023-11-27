#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Event.hpp"

#include <cereal/access.hpp>
#include <cereal/types/vector.hpp>

namespace Engine {
	class Asset;
	class AssetImportSettings;

	class ENGINE_LOCAL AssetRegistry {

		struct AssetMetaInfo {
		private:
			friend class cereal::access;

		public:
			xg::Guid guid;
			xg::Guid parentGuid;
			std::vector<xg::Guid> childGuids;
			xg::Guid importSettingsGuid;

			std::string fullPath;
			std::string assetNameWithExtension;

			void AddChildGuid(xg::Guid childGuid);

			bool operator==(const AssetMetaInfo& other) const;
			bool operator!=(const AssetMetaInfo& other) const;

		private:
			template<class Archive>
			void Serialize(Archive& archive);
		};

		friend class AssetManager;

		AssetRegistry() = default;

	public:
		Sharp::Event<const std::shared_ptr<Asset>&, const std::string&> OnAssetRegisteredEvent;
		Sharp::Event<const xg::Guid&, const std::string&> OnAssetUnRegisteredEvent;
		Sharp::Event<const xg::Guid&, const std::string&, const std::string&, const std::string&, const std::string&> OnAssetMovedOrRenamedEvent;

		AssetRegistry(const AssetRegistry& other) = delete;
		AssetRegistry(AssetRegistry&& other) noexcept = delete;
		~AssetRegistry() = default;

		AssetRegistry& operator=(const AssetRegistry& other) = delete;
		AssetRegistry& operator=(AssetRegistry&& other) noexcept = delete;

		static bool IsAssetRegistered(const xg::Guid& assetGuid);

		static bool TryGetFullPathForGuid(const xg::Guid& assetGuid, std::string& fullPath, std::string& assetNameWithExtension);

		static bool TryGetGuidForPath(const std::string& pathInProject, const char* assetNameWithExtension, xg::Guid& assetGuid);

		static bool TryGetGuidForFullPath(const std::string& fullPath, const char* assetNameWithExtension, xg::Guid& assetGuid);

		bool TryRegisterAsset(const std::shared_ptr<Asset>& assetToRegister, const xg::Guid& importSettingsGuid, const std::string& fullPath, const char* assetNameWithExtension);

		bool TryUnRegisterAsset(const xg::Guid& assetGuid);

		bool TryUpdatePathForGuid(const xg::Guid& assetGuid, const std::string& newFullPath, const char* newAssetNameWithExtension);

		static bool TryWriteCustomDataToAsset(const xg::Guid& assetGuid, const std::shared_ptr<Asset>& customData, const std::string& dataExtension);

		static bool TryLoadCustomDataFromAsset(const xg::Guid& assetGuid, const std::string& dataExtension, std::shared_ptr<Asset>& customData);

		static bool TryAddChildAssetToAsset(const xg::Guid& parentGuid, const xg::Guid& childGuid);

		static void RemoveChildAssetFromAsset(const xg::Guid& parentGuid, const xg::Guid& childGuid);

		static bool TryAssignGuidToAsset(const std::shared_ptr<Asset>& assetToUpdate, const xg::Guid& assetGuid);

		static bool TryGetParentForAsset(const xg::Guid& childGuid, xg::Guid& parentGuid);

		static bool TryGetChildrenForAsset(const xg::Guid& parentGuid, std::vector<xg::Guid>& childGuids);

		static bool TryGetImportSettingsForAsset(const xg::Guid& assetGuid, std::shared_ptr<AssetImportSettings>& importSettings);

		[[nodiscard]] static std::string GetAssetRegistryPathForGuid(const xg::Guid& assetGuid);

	private:
		[[nodiscard]] static bool TryLoadAssetMetaInfoForGuid(const xg::Guid& assetGuid, AssetMetaInfo& outputMetaInfo);

		static void SaveAssetMetaInfo(const AssetMetaInfo& assetMetaInfo);

		static void DeleteAssetMetaInfo(const AssetMetaInfo& assetMetaInfo);
	};

	template <class Archive>
	void AssetRegistry::AssetMetaInfo::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(guid),
			CEREAL_NVP(parentGuid),
			CEREAL_NVP(childGuids),
			CEREAL_NVP(importSettingsGuid),
			CEREAL_NVP(fullPath),
			CEREAL_NVP(assetNameWithExtension)
		);
	}
}
