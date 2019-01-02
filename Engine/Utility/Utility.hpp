#pragma once

#include "Engine/api.hpp"
#include "ThirdParty/EASTL-master/include/EASTL/vector.h"
#include "ThirdParty/EASTL-master/include/EASTL/string.h"

namespace Engine
{
	class ENGINE_API Utility
	{
	public:
		Utility() = default;
		~Utility() = default;

		static eastl::vector<char> ReadFile(const eastl::string& fileName, int fileOpenMode = 1);
		static bool FileExists(const eastl::string& fileName);
	};
} // namespace Engine