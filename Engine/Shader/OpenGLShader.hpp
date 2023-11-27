#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Api/Api.hpp"
#include "Engine/Shader/Shader.hpp"

#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace Engine
{
	class Texture;
	class Color;

	/// <summary>
	/// This object is used to store information regarding the OpenGLShader.
	/// </summary>
	class ENGINE_API OpenGLShader
	{
		friend class OpenGLRenderer;

		GLuint program = 0;
		GLint attributeBufferSize = 0;
		std::map<std::shared_ptr<ShaderParameter>, GLint> locationByParameter {};
		std::map<std::shared_ptr<ShaderParameter>, GLenum> typeByParameter {};
		std::map<std::shared_ptr<ShaderParameter>, GLint> sizeByParameter {};
		std::map<std::shared_ptr<ShaderParameter>, GLuint> offsetByParameter {};
		std::vector<std::shared_ptr<ShaderParameter>> uniforms {};
		std::vector<std::shared_ptr<ShaderParameter>> attributes {};
		std::vector<std::shared_ptr<ShaderParameter>> dirtyParameters {};

		explicit OpenGLShader(const std::shared_ptr<Shader>& shader);

	public:
		virtual ~OpenGLShader();
		OpenGLShader(const OpenGLShader& other) = delete;
		OpenGLShader(OpenGLShader&& other) noexcept = delete;
		OpenGLShader& operator=(const OpenGLShader& other) = delete;
		OpenGLShader& operator=(OpenGLShader&& other) noexcept = delete;

		void Activate() const;

		void ProcessDirtyShaderParameters();

		void EnableAttributes() const;

		void DisableAttributes() const;

	private:

		void LoadParameters(const std::shared_ptr<Shader>& shader);

		void ExtractParameters(const std::shared_ptr<Shader>& shader) const;

		void HandleOnShaderParameterValueChangedEvent(const std::shared_ptr<ShaderParameter>& shaderParameter);
	};
} //namespace Engine
#endif