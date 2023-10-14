#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/types/base_class.hpp>

namespace Engine {
	class ENGINE_LOCAL AssetImportSettings : public Asset {
		friend cereal::access;

	public:
		xg::Guid relatedAssetGuid;

		explicit AssetImportSettings() = default;
		virtual ~AssetImportSettings() override = default;
		AssetImportSettings(const AssetImportSettings& other) = delete;
		AssetImportSettings(AssetImportSettings&& other) = delete;
		AssetImportSettings& operator=(const AssetImportSettings& other) = delete;
		AssetImportSettings& operator=(AssetImportSettings&& other) = delete;

	private:

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void AssetImportSettings::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(relatedAssetGuid)
		);
	}
}