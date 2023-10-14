#pragma once

#include "Engine/Api/Api.hpp"

#include <string>

namespace Engine {
	class ENGINE_API StringUtility {
	public:
		static void SanitizePath(std::string& path);
		static std::string SanitizePath(const std::string& path);
	};
}

