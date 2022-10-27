#include "Engine/AssetManagement/Asset.hpp"

namespace Engine {
	const std::string& Asset::GetName() const {
		return name;
	}

	void Asset::SetName(const std::string& newName) {
		name = newName;
	}

	const xg::Guid& Asset::GetGuid() const {
		return guid;
	}
}
