#include "Engine/Shader/Shader.hpp"
#include "Engine/engine.hpp"


namespace Engine
{
	Shader::Shader(const eastl::string& vertexFileName, const eastl::string& fragmentFileName)
	{
		/*Engine::GetFileSystem().SetWorkingDirectory("Resources/Shaders");

		FileSystems::File& file = Engine::GetFileSystem().LoadFile(vertexFileName.c_str());
		std::string shaderText = file.ReadString();
		Engine::GetFileSystem().UnLoadFile(file);


		vertexData.clear();
		vertexData.resize(shaderText.length() + 1);
		eastl::copy(shaderText.c_str(), shaderText.c_str() + shaderText.length() + 1, shaderText.begin());

		shaderText = Engine::GetFileSystem().LoadFile(fragmentFileName.c_str()).ReadString();
		fragmentData.clear();*/
		
	}

	void Shader::Activate()
	{
	}

	void Shader::Deactivate()
	{
	}
}
