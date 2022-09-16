#include "Engine/AssetManagement/Asset.hpp"

#include <combaseapi.h>
#include <cereal/details/helpers.hpp>

namespace Engine {

	Asset::Asset() : Asset({}, {}) {}

	Asset::Asset(std::string name, std::string path) : name(std::move(name)), path(std::move(path)) {

		CoCreateGuid(&guid);
		
	}

	const std::string& Asset::GetName() const {
		return name;
	}

	void Asset::SetName(const std::string& newName) {
		name = newName;
	}

	const std::string& Asset::GetPath() const {
		return path;
	}

	void Asset::SetPath(const std::string& newPath) {
		path = newPath;
	}

	const GUID& Asset::GetGuid() const {
		return guid;
	}
}
