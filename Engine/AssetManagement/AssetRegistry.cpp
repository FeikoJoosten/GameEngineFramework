#include "Engine/AssetManagement/AssetRegistry.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Utility/Logging.hpp"

#include <nameof.hpp>
#include <crossguid/guid.hpp>
#include <cereal/types/polymorphic.hpp>

namespace Engine {

	std::shared_ptr<Asset> AssetRegistry::LoadAsset(const xg::Guid assetGuid) {
		const std::map<std::string, std::string>::iterator iterator = assetRegistryPathsByGuid.find(assetGuid.str());

		if (iterator == assetRegistryPathsByGuid.end()) {
			DEBUG_ERROR("Failed to find Asset for Guid");
			return {};
		}

		return {};
		//return AssetManager::Get()->ReadDataFromPath<std::shared_ptr<Asset>>(iterator->second);
	}

	void AssetRegistry::RegisterAsset(const std::shared_ptr<Asset>& assetToRegister) {
		const std::map<std::string, std::string>::iterator iterator = assetRegistryPathsByGuid.find(assetToRegister->GetGuid().str());

		if (iterator != assetRegistryPathsByGuid.end()) {
			DEBUG_ERROR("Asset is already registerd!");
			return;
		}

		assetToRegister->guid = xg::newGuid();
		std::string savePath = static_cast<std::string>(NAMEOF_SHORT_TYPE(AssetRegistry)) + "/" + assetToRegister->guid.str();
		assetRegistryPathsByGuid.insert({ assetToRegister->guid.str(), savePath });
		//AssetManager::Get()->WriteDataToPath(savePath, assetToRegister);
	}
}
