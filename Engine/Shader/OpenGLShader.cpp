
#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Shader/OpenGLShader.hpp"
#include "Engine/Texture/Texture.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Utility/Logging.hpp"
#include "Engine/Utility/Utility.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"

#include <cassert>
#include <ios>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{

	////////////////////////////////////////////////////////////////////////////////
	// Compile shader and report success or failure
	////////////////////////////////////////////////////////////////////////////////
	bool CompileShader(GLuint* shader, GLenum type, const GLchar* source)
	{
		GLint status;

		if (!source)
		{
			//LOG("Failed to load shader %s \n", source);
			return false;
		}

		*shader = glCreateShader(type);
		glShaderSource(*shader, 1, &source, nullptr);
		glCheckError();
		glCompileShader(*shader);

		GLint logLength = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 1)
		{
			GLchar *log = static_cast<GLchar *>(malloc(logLength));
			glGetShaderInfoLog(*shader, logLength, &logLength, log);
			const std::string logInfo = log;
			debug_info("OpenGLShader", "CompileShader", std::string("Shader compile log: \n " + logInfo));
			free(log);
		}

		glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			glDeleteShader(*shader);
			return false;
		}

		glCheckError();

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Link Program
	////////////////////////////////////////////////////////////////////////////////
	bool LinkProgram(GLuint prog)
	{
		GLint status;

		glLinkProgram(prog);

		GLint logLength = 0;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 1)
		{
			GLchar *log = static_cast<GLchar *>(malloc(logLength));
			glGetProgramInfoLog(prog, logLength, &logLength, log);
			std::string logInfo = log;
			debug_info("OpenGLShader", "LinkProgram", std::string("Program link log: \n " + logInfo));
			free(log);
		}

		glGetProgramiv(prog, GL_LINK_STATUS, &status);
		if (status == 0)
			return false;

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Validate program
	////////////////////////////////////////////////////////////////////////////////
	bool ValidateProgram(GLuint prog)
	{
		GLint logLength = 0;
		GLint status = 0;

		glValidateProgram(prog);
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			GLchar *log = static_cast<GLchar *>(malloc(logLength));
			glGetProgramInfoLog(prog, logLength, &logLength, log);
			free(log);
		}

		logLength = 0;
		glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
		if (status == 0)
		{
			return false;
		}
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	//                              ShaderParameter
	//
	////////////////////////////////////////////////////////////////////////////////

	void ShaderParameter::SetValue(float val) const
	{
		if (!IsValid())
			return;


		assert(_type == GL_FLOAT);
		glUniform1f(_location, val);
		glCheckError();
	}

	void ShaderParameter::SetValue(int val) const
	{
		if (!IsValid())
			return;

		assert(_type == GL_INT);
		glUniform1i(_location, val);
		glCheckError();
	}

	void ShaderParameter::SetValue(bool val) const
	{
		if (!IsValid())
			return;

		assert(_type == GL_BOOL);
		glUniform1i(_location, val);
		glCheckError();
	}

	void ShaderParameter::SetValue(const glm::vec2& vec) const
	{
		if (!IsValid())
			return;

		assert(_type == GL_FLOAT_VEC2);
		glUniform2fv(_location, 1, &vec.x);
		glCheckError();
	}

	void ShaderParameter::SetValue(const glm::vec3& vec) const
	{
		if (!IsValid())
			return;

		assert(_type == GL_FLOAT_VEC3);
		glUniform3fv(_location, 1, glm::value_ptr(vec));
		glCheckError();
	}

	void ShaderParameter::SetValue(const glm::vec4& vec) const
	{
		if (!IsValid())
			return;

		assert(_type == GL_FLOAT_VEC4);
		glUniform4fv(_location, 1, &vec.x);
		glCheckError();
	}

	void ShaderParameter::SetValue(const glm::mat4x4& mtx, bool transpose) const
	{
		if (!IsValid())
			return;

		assert(_type == GL_FLOAT_MAT4);
		glUniformMatrix4fv(_location, 1, transpose, glm::value_ptr(mtx));
		glCheckError();
	}

	void ShaderParameter::SetValue(Texture &texture_) const
	{
		if (!IsValid())
			return;

		assert(_type == GL_SAMPLER_2D);
		// Use texture with index sampler. GL_TEXTURE1 = GL_TEXTURE10+1 is always true
		glActiveTexture(GL_TEXTURE0 + _sampler);
		glCheckError();
		// Work with this texture
		glBindTexture(GL_TEXTURE_2D, GLuint(texture_.GetTexture()));
		glCheckError();
		// Set the sampler
		glUniform1i(_location, _sampler);
		glCheckError();
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	//                          ShaderAttribute
	//
	////////////////////////////////////////////////////////////////////////////////

	void ShaderAttribute::SetAttributePointer(GLint size,
		GLenum type,
		GLboolean normalized,
		GLsizei stride,
		const GLvoid *pointer) const
	{
		if (!IsValid())
			return;

		glVertexAttribPointer(
			_location,           // attribute
			size,               // number of elements per vertex element
			type,               // the type of each element
			normalized,         // take our values as-is or normalize
			stride,             // no extra data between each position
			pointer             // offset of first element
		);
		glCheckError();

		glEnableVertexAttribArray(_location);
		glCheckError();
	}

	void ShaderAttribute::DisableAttributePointer() const
	{
		if (!IsValid())
			return;

		glDisableVertexAttribArray(_location);
		glCheckError();
	}

	////////////////////////////////////////////////////////////////////////////////
	//
	//                                  Shader
	//
	////////////////////////////////////////////////////////////////////////////////

	GLuint LoadShader(std::string filePath, int shaderType)
	{
		GLuint shader;
		CompileShader(&shader, shaderType, reinterpret_cast<char const *const>(Utility::ReadFile(filePath, std::ios::binary).data()));
		return shader;
	}

	OpenGLShader::OpenGLShader(const std::string& vertexFileName, const std::string& fragmentFileName) :
		OpenGLShader(vertexFileName, fragmentFileName, "")
	{}

	OpenGLShader::OpenGLShader(const std::string& vertexFileName,
		const std::string& fragmentFileName,
		const std::string& geometryFileName)
	{
		if (!LoadSource(vertexFileName, fragmentFileName, geometryFileName))
			return;
	}

	bool OpenGLShader::LoadSource(const std::string& vertexShader, const std::string& fragmentShader,
		const std::string& geometryShader)
	{
		program = glCreateProgram();
		
		GLuint vertShader = LoadShader(shaderFolder + vertexShader, GL_VERTEX_SHADER);
		GLuint fragShader = LoadShader(shaderFolder + fragmentShader, GL_FRAGMENT_SHADER);
		GLuint geomShader = 0;

		if (geometryShader.length() > 0)
			geomShader = LoadShader(shaderFolder + geometryShader, GL_GEOMETRY_SHADER);

		// Attach vertex shader to program
		glAttachShader(program, vertShader);
		glCheckError();

		// Attach geometry shader to program
		if (geomShader)
			glAttachShader(program, geomShader);

		// Attach fragment shader to program
		glAttachShader(program, fragShader);
		glCheckError();

		// Link program
		if (!LinkProgram(program))
		{
			if (vertShader)
			{
				glDeleteShader(vertShader);
				vertShader = 0;
			}
			if (fragShader)
			{
				glDeleteShader(fragShader);
				fragShader = 0;
			}
			if (geomShader)
			{
				glDeleteShader(geomShader);
				geomShader = 0;
			}
			if (program)
			{
				glDeleteProgram(program);
				program = 0;
			}

			glCheckError();

			// We crash here, else the logs will be flooded with repeated
			// error messages.
			_ASSERT(false);

			return false;
		}

		glDeleteShader(vertShader);
		glCheckError();

		glDeleteShader(geomShader);
		glCheckError();

		glDeleteShader(fragShader);
		glCheckError();

		LoadParameters();

		return true;
	}

	std::shared_ptr<ShaderParameter> OpenGLShader::GetParameter(const std::string& name)
	{
		// Try to find param
		auto itr = parameters.find(name);
		if (itr != parameters.end())
			return itr->second;

		// Create and return a non-valid param that is stored in collection
		// in case it becomes valid after a reload
		parameters[name] = std::shared_ptr<ShaderParameter>();
		return parameters[name];
	}

	std::shared_ptr<ShaderAttribute> OpenGLShader::GetAttribute(const std::string& name)
	{
		// Try to find param
		auto itr = attributes.find(name);
		if (itr != attributes.end())
			return itr->second;

		// Create and return a non-valid param that is stored in collection
		// in case it becomes valid after a reload
		attributes[name] = std::shared_ptr<ShaderAttribute>();
		return attributes[name];
	}

	GLuint OpenGLShader::GetProgram() const
	{
		return program;
	}

	void OpenGLShader::Activate()
	{
		glUseProgram(program);
		glCheckError();
	}

	void OpenGLShader::Deactivate()
	{
		for (auto& a : attributes)
			a.second->DisableAttributePointer();
	}

	void OpenGLShader::LoadParameters()
	{
		/// The shader should invalidate when reloading a new shader file
		/// a some information can be old
		for (auto& itr : parameters)
			itr.second->Invalidate();

		// Get the number of uniforms and resize the parameters collection accordingly
		GLint numActiveUniforms = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
		//parameters.resize(numActiveUniforms);

		// Get the maximum name length
		GLint maxUniformNameLength = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
		std::vector<GLchar> uniformNameData(maxUniformNameLength);

		// Go over all the uniforms
		int samplerCount = 0;
		for (int uniform = 0; uniform < numActiveUniforms; uniform++)
		{
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;
			glGetActiveUniform(
				program,
				uniform,
				maxUniformNameLength,
				&actualLength,
				&arraySize,
				&type,
				&uniformNameData[0]);
			std::string name(&uniformNameData[0], actualLength);
			GLint location = glGetUniformLocation(program, name.c_str());

			auto itr = parameters.find(name);
			if (itr != parameters.end())
			{
				if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
					itr->second->Reset(this, name, type, location, samplerCount++);
				else
					itr->second->Reset(this, name, type, location);
			}
			else
			{
				std::shared_ptr<ShaderParameter> param;
				if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
					param = std::shared_ptr<ShaderParameter>(new ShaderParameter(this, name, type, location, samplerCount++));
				else
					param = std::shared_ptr<ShaderParameter>(new ShaderParameter(this, name, type, location));
				parameters[name] = param;
			}
		}


		GLint numActiveAttribs = 0;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);


		GLint maxAttribNameLength = 0;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
		std::vector<GLchar> attribNameData(maxAttribNameLength);


		for (int attrib = 0; attrib < numActiveAttribs; ++attrib)
		{
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;
			glGetActiveAttrib(program,
				attrib,
				GLsizei(attribNameData.size()),
				&actualLength,
				&arraySize,
				&type,
				&attribNameData[0]);
			std::string name(static_cast<char*>(&attribNameData[0]));
			GLint location = glGetAttribLocation(program, name.c_str());

			auto itr = attributes.find(name);
			if (itr != attributes.end())
			{
				itr->second->Reset(this, name, type, location);
			}
			else
			{
				attributes[name] = std::shared_ptr<ShaderAttribute>(new ShaderAttribute(this, name, type, location));
			}
		}
	}

	bool OpenGLShader::Validate() const
	{
		if (!ValidateProgram(program))
		{
			//LOG("Failed to validate program: %d", _program);
			glCheckError();
			return false;
		}
		return true;
	}
} //namespace Engine
#endif