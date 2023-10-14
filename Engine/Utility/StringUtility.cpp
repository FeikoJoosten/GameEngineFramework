#include "Engine/Utility/StringUtility.hpp"

#include <filesystem>

namespace Engine {
	void StringUtility::SanitizePath(std::string& path) {
		const std::string inPath = path;
		path = SanitizePath(inPath);
	}

	std::string StringUtility::SanitizePath(const std::string& path) {
		const std::filesystem::path sanitizedPath = std::filesystem::path(path, std::filesystem::path::generic_format).lexically_normal();
		return sanitizedPath.generic_string();
	}
}
