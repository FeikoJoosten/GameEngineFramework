#pragma once

#include "Engine/Api.hpp"

#include <string>
#include <vector>

namespace Engine
{
	/// <summary>
	/// This object is used to store information regarding the shader. 
	/// </summary>
	class ENGINE_API Shader
	{
	private:
		friend class Renderer;
#ifdef USING_OPENGL
		friend class OpenGLRenderer;
		friend class OpenGLShader;
#endif
#ifdef USING_VULKAN
		friend class VulkanRenderer;
#endif

		/// <summary>
		/// Default constructor.
		/// </summary>
		Shader() = default;
		/// <summary>
		/// Create a shader with vertex geometry and fragment programs.
		/// </summary>
		/// <param name="vertexFileName">The vertex shader file you want to load.
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		/// <param name="fragmentFileName">The fragment shader file you want to load. 
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		Shader(const std::string& vertexFileName, const std::string& fragmentFileName);
		//Shader(const Shader& other) = delete;
		/// <summary>
		/// Default deconstructor.
		/// </summary>
		virtual ~Shader() = default;

	public:
		/// <summary>
		/// Call this method before you start rendering.
		/// </summary>
		virtual void Activate();

		/// <summary>
		/// Call this method after you've finished rendering.
		/// </summary>
		virtual void Deactivate();
	private:
		std::vector<char> vertexData;
		std::vector<char> fragmentData;
	protected:
		const char* shaderFolder = "Resources/Shaders/";
	};
}