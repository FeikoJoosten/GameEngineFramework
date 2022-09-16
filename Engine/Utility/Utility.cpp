#include "Engine/Utility/Utility.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"

#include <fstream>

namespace Engine
{
	std::vector<char> Utility::ReadFile(const std::string& fileName, const int fileOpenMode)
	{
		std::ifstream file((AssetManager::Get()->GetProjectRoot() + fileName).c_str());

		if (!file)
			throw std::runtime_error("Failed to find file!");

		if (!file.is_open())
			throw std::runtime_error("Failed to open file!");

		file.seekg(0, std::ifstream::end);
		const std::streampos fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0, std::ifstream::beg);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	bool Utility::FileExists(const std::string& fileName)
	{
		std::ifstream file((AssetManager::Get()->GetProjectRoot() + fileName).c_str());

		const bool fileExists = file.is_open();
		file.close();

		return fileExists;
	}
} // namespace Engine
