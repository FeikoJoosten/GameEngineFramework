#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <crossguid/guid.hpp>

namespace Engine {
	class ENGINE_API Asset : public std::enable_shared_from_this<Asset> {
		friend cereal::access;
		friend class AssetRegistry;

		std::string name {};
		xg::Guid guid;

	protected:
		explicit Asset() = default;
		explicit Asset(std::string name);

	public:

		virtual ~Asset() = default;
		Asset(const Asset& other) = delete;
		Asset(Asset&& other) = delete;
		Asset& operator=(const Asset& other) = delete;
		Asset& operator=(Asset&& other) = delete;

		[[nodiscard]] const std::string& GetName() const;

		void SetName(const std::string& newName);

		[[nodiscard]] const xg::Guid& GetGuid() const;

		void Save();

	private:

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void Asset::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(name),
			CEREAL_NVP(guid)
		);
	}
}