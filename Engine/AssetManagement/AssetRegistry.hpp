#pragma once

#include "Engine/Api/Api.hpp"

#include <cereal/access.hpp>
#include <cereal/types/map.hpp>
#include <crossguid/guid.hpp>

namespace Engine {
	class Asset;

	class ENGINE_LOCAL AssetRegistry {
		friend cereal::access;

		AssetRegistry() = default;

	public:
		~AssetRegistry() = default;
		AssetRegistry(const AssetRegistry& other) = delete;
		AssetRegistry(AssetRegistry&& other) = delete;
		AssetRegistry& operator=(const AssetRegistry& other) = delete;
		AssetRegistry& operator=(AssetRegistry&& other) = delete;

		std::shared_ptr<Asset> LoadAsset(xg::Guid assetGuid);
		
		void RegisterAsset(const std::shared_ptr<Asset>& assetToRegister);

	private:
		std::map<std::string, std::string> assetRegistryPathsByGuid {};
		
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

