#pragma once

#include "Engine/api.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/string.h>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	class ENGINE_API Shader
	{
	public:
		Shader() = default;
		/// Create a shader with vertex geometry and fragment programs 
		Shader(const eastl::string& vertexFileName, const eastl::string& fragmentFileName);
		//Shader(const Shader& other) = delete;
		virtual ~Shader() = default;
		
		/// Call before drawing 
		virtual void Activate();

		/// Call after drawing
		virtual void Deactivate();
	private:
		eastl::vector<char> vertexData;
		eastl::vector<char> fragmentData;
	protected:
		const char* shaderFolder = "Resources/Shaders";
	};
}