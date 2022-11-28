#pragma once

#include "Engine/Api/Api.hpp"

#include <memory>
#include <string>

namespace Engine {
	class Asset;

	class ENGINE_API IAssetImporter {
	public:
		virtual bool SupportsFileExtension(const std::string& fileExtension) = 0;

		virtual std::shared_ptr<Asset> ProcessAsset(const std::string& fullPath, const std::string& assetName) = 0;
	};
}
