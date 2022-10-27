#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Event.hpp"

#include <cereal/access.hpp>
#include <cereal/types/map.hpp>
#include <crossguid/guid.hpp>

namespace Engine {
	class Asset;

	class ENGINE_LOCAL AssetRegistry {
		friend cereal::access;
		friend class AssetManager;

		std::map<xg::Guid, std::string> assetRegistryPathsByGuid {};

		AssetRegistry() = default;

	public:
		Sharp::Event<const std::shared_ptr<Asset>&, const std::string&> OnAssetRegisteredEvent;
		Sharp::Event<const xg::Guid&, const std::string&> OnAssetUnRegisteredEvent;
		Sharp::Event<const xg::Guid&, const std::string&, const std::string&> OnAssetMovedOrRenamedEvent;

		AssetRegistry(const AssetRegistry& other) = delete;
		AssetRegistry(AssetRegistry&& other) noexcept = delete;
		~AssetRegistry() = default;

		AssetRegistry& operator=(const AssetRegistry& other) = delete;
		AssetRegistry& operator=(AssetRegistry&& other) noexcept = delete;

		bool TryGetPathForGuid(xg::Guid assetGuid, std::string& assetPath) const;

		bool TryGetGuidForPath(const std::string& assetPath, xg::Guid& assetGuid) const;

		bool TryRegisterAsset(const std::shared_ptr<Asset>& assetToRegister, const std::string& assetPath);

		bool TryUnRegisterAsset(const xg::Guid& assetGuid);

		bool TryUpdatePathForGuid(const xg::Guid& assetGuid, const std::string& newAssetPath);

	private:
		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void AssetRegistry::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(assetRegistryPathsByGuid)
		);
	}
}

