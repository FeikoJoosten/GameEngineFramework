#pragma once

#include "Engine/Api.hpp"

#include <vector>
#include <string>

namespace Engine
{
	class ENGINE_API Utility
	{
	public:
		static std::vector<char> ReadFile(const std::string& fileName, int fileOpenMode = 1);
		static bool FileExists(const std::string& fileName, bool isFullPath = false);

		static std::string GetDirectoryFromPath(const std::string& path);
	};
} // namespace Engine