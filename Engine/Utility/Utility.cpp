#include "Utility.hpp"
#include <fstream>
#include <iostream>

namespace Engine
{
	eastl::vector<char> Utility::ReadFile(const eastl::string& fileName, int fileOpenMode)
	{
		std::ifstream file(fileName.c_str());

		if (!file)
		{
			throw std::runtime_error("Failed to find file!");
		}

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file!");
		}

		file.seekg(0, file.end);
		size_t fileSize = size_t(file.tellg());
		eastl::vector<char> buffer(fileSize);

		file.seekg(0, file.beg);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	bool Utility::FileExists(const eastl::string& fileName)
	{
		std::ifstream file(fileName.c_str());

		bool fileExists = file.is_open();
		file.close();

		return fileExists;
	}
} // namespace Engine
