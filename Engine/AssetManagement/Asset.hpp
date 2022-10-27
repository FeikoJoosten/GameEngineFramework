#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetRegistry.hpp"

#include <cereal/access.hpp>
#include <cereal/types/string.hpp>
#include <crossguid/guid.hpp>

namespace Engine {
	class ENGINE_API Asset {
		friend cereal::access;
		friend void AssetRegistry::RegisterAsset(const std::shared_ptr<Asset>& assetToRegister);

		std::string name {};
		xg::Guid guid {};

	protected:
		explicit Asset() = default;

	public:
		virtual ~Asset() = default;
		Asset(const Asset& other) = delete;
		Asset(Asset&& other) = delete;
		Asset& operator=(const Asset& other) = delete;
		Asset& operator=(Asset&& other) = delete;

		[[nodiscard]] const std::string& GetName() const;

		void SetName(const std::string& newName);

		[[nodiscard]] const xg::Guid& GetGuid() const;

	private:

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void Asset::Serialize(Archive& archive) {
		archive(CEREAL_NVP(name));
	}
}
