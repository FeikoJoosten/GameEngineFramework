#include "Engine/Platform/OpenGL/Shader/OpenGLShader.hpp"
#include "Engine/Utility/Defines.hpp"

#ifdef USING_OPENGL
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Platform/OpenGL/Renderer/OpenGLRenderer.hpp"
#include "Engine/Platform/OpenGL/Renderer/OpenGLUtility.hpp"
#include "Engine/Platform/OpenGL/Texture/OpenGLTexture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Shader/ShaderStage.hpp"
#include "Engine/Texture/Texture.hpp"
#include "Engine/Utility/Logging.hpp"

#include <cassert>
#include <nameof.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ranges>

namespace Engine {

	bool CompileShader(GLuint& shader, const GLenum& type, const GLchar* source) {
		GLint status;

		if (!source) {
			//LOG("Failed to load shader %s \n", source);
			return false;
		}

		shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, nullptr);
		glCheckError();
		glCompileShader(shader);

		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 1) {
			GLchar* log = static_cast<GLchar*>(malloc(logLength));
			glGetShaderInfoLog(shader, logLength, &logLength, log);
			const std::string logInfo = log;
			DEBUG_INFO(std::string("Shader compile log: \n " + logInfo));
			free(log);
		}

		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == 0) {
			glDeleteShader(shader);
			return false;
		}

		glCheckError();

		return true;
	}

	bool LinkProgram(const GLuint& program) {
		glLinkProgram(program);

		GLint logLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0) {
			GLchar* log = static_cast<GLchar*>(malloc(logLength));
			glGetProgramInfoLog(program, logLength, &logLength, log);
			const std::string logInfo = log;
			DEBUG_INFO(std::string("Program link log: \n " + logInfo));
			free(log);
		}

		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		return status != 0;
	}

	bool TryGetShaderParameterType(const GLenum type, ShaderParameterType& parameterType) {
		switch (type) {
		case GL_FLOAT:
			parameterType = ShaderParameterType::Float;
			return true;
		case GL_INT:
			parameterType = ShaderParameterType::Int;
			return true;
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
			parameterType = ShaderParameterType::Vector;
			return true;
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
			parameterType = ShaderParameterType::Matrix;
			return true;
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
			parameterType = ShaderParameterType::Texture;
			return true;
		default:
			parameterType = ShaderParameterType::Float;
			DEBUG_ERROR("Unsupported parameter type! Type: " + std::to_string(type));
			return false;
		}
	}

	OpenGLShader::OpenGLShader(const std::shared_ptr<Shader>& shader) {
		program = glCreateProgram();

		std::vector<std::shared_ptr<ShaderStage>> shaderStages {};
		shader->GetShaderStages(shaderStages);

		std::vector<GLuint> shaderStageIds {};
		for (const std::shared_ptr<ShaderStage>& shaderStage : shaderStages) {
			GLenum shaderStageType;
			switch (shaderStage->shaderStageType) {
			case ShaderStageType::Vertex:
				shaderStageType = GL_VERTEX_SHADER;
				break;
			case ShaderStageType::Fragment:
				shaderStageType = GL_FRAGMENT_SHADER;
				break;
			case ShaderStageType::Geometry:
				shaderStageType = GL_GEOMETRY_SHADER;
				break;
			case ShaderStageType::Compute:
				shaderStageType = GL_COMPUTE_SHADER;
				break;
			default:
				DEBUG_ERROR("Unknown shader stage, skipping! Stage: " + std::string(NAMEOF_ENUM(shaderStage->shaderStageType)));
				continue;
			}

			GLuint shaderStageId;
			if (!CompileShader(shaderStageId, shaderStageType, shaderStage->shaderStageCode.data())) continue;

			shaderStageIds.push_back(shaderStageId);

			glAttachShader(program, shaderStageId);
			glCheckError();
		}

		if (!LinkProgram(program)) {
			for (const GLuint& shaderStageId : shaderStageIds)
				glDeleteShader(shaderStageId);

			shaderStageIds.clear();

			if (program) glDeleteProgram(program);
			program = 0;
			glCheckError();

			return;
		}

		for (const GLuint& stageId : shaderStageIds) {
			glDeleteShader(stageId);
			glCheckError();
		}
		shaderStageIds.clear();
	}

	OpenGLShader::~OpenGLShader() {
		for (const std::shared_ptr<ShaderParameter>& uniform : uniforms)
			uniform->OnParameterValueChangedEvent -= Sharp::EventHandler::Bind(this, &OpenGLShader::HandleOnShaderParameterValueChangedEvent);

		DisableAttributes();

		dirtyParameters.clear();
		uniforms.clear();
		attributes.clear();
		locationByParameter.clear();
		typeByParameter.clear();
		sizeByParameter.clear();

		program = 0;
	}

	void OpenGLShader::Activate() const {
		if (!program) return;

		glUseProgram(program);
		glCheckError();
	}

	void OpenGLShader::ProcessDirtyShaderParameters() {
		const std::shared_ptr<OpenGLRenderer>& openGlRenderer = std::static_pointer_cast<OpenGLRenderer>(Renderer::Get());

		for (const std::shared_ptr<ShaderParameter>& parameter : dirtyParameters) {
			const GLenum parameterType = typeByParameter.at(parameter);
			const GLint location = locationByParameter.at(parameter);

			if (location == -1) continue; // Should this log an error?

			switch (parameterType) {
			case GL_FLOAT:
				glUniform1f(location, parameter->GetFloatValue());
				glCheckError();
				break;
			case GL_INT:
			case GL_BOOL:
				glUniform1i(location, parameter->GetIntValue());
				glCheckError();
				break;
			case GL_FLOAT_VEC2:
				glUniform2fv(location, 1, value_ptr(parameter->GetVectorValue()));
				glCheckError();
				break;
			case GL_FLOAT_VEC3:
				glUniform3fv(location, 1, value_ptr(parameter->GetVectorValue()));
				glCheckError();
				break;
			case GL_FLOAT_VEC4:
				glUniform4fv(location, 1, value_ptr(parameter->GetVectorValue()));
				glCheckError();
				break;
			case GL_FLOAT_MAT4:
				glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(parameter->GetMatrixValue()));
				glCheckError();
				break;
			case GL_SAMPLER_2D:
			{
				std::shared_ptr<OpenGLTexture> openGlTexture;
				if (!openGlRenderer || !openGlRenderer->TryGetTextureForGuid(parameter->GetTextureValue()->GetGuid(), openGlTexture)) {
					DEBUG_ERROR("Failed to assign texture, because it is not yet registered to the renderer!");
					break;
				}
				glActiveTexture(GL_TEXTURE0); // What if there is more than 1 texture?
				glCheckError();
				glBindTexture(GL_TEXTURE_2D, openGlTexture->GetTextureId());
				glCheckError();
				glUniform1i(location, 0);
				glCheckError();
				break;
			}
			default:
				DEBUG_ERROR("Not supported type! " + std::to_string(parameterType));
				break;
			}
		}

		dirtyParameters.clear();
	}

	void OpenGLShader::EnableAttributes() const {
		for (const std::shared_ptr<ShaderParameter>& attribute : attributes) {
			const GLint& location = locationByParameter.at(attribute);
			const GLint& size = sizeByParameter.at(attribute);
			const GLuint& offset = offsetByParameter.at(attribute);
			glVertexAttribPointer(
				location,							// attribute
				size,								// number of elements per vertex element
				typeByParameter.at(attribute),		// the type of each element
				GL_FALSE,							// take our values as-is or normalize
				attributeBufferSize,				// no extra data between each position
				reinterpret_cast<void*>(offset)		// offset of first element  // NOLINT(performance-no-int-to-ptr)
			);
			glCheckError();

			glEnableVertexAttribArray(location);
			glCheckError();
		}
	}

	void OpenGLShader::DisableAttributes() const {
		for (const std::shared_ptr<ShaderParameter>& attribute : attributes) {
			const GLint& location = locationByParameter.at(attribute);
			if (location == -1) continue;

			glDisableVertexAttribArray(location);
			glCheckError();
		}
	}

	void OpenGLShader::LoadParameters(const std::shared_ptr<Shader>& shader) {
		for (const std::shared_ptr<ShaderParameter>& uniform : uniforms)
			uniform->OnParameterValueChangedEvent -= Sharp::EventHandler::Bind(this, &OpenGLShader::HandleOnShaderParameterValueChangedEvent);

		locationByParameter.clear();
		typeByParameter.clear();
		sizeByParameter.clear();
		uniforms.clear();
		attributes.clear();
		dirtyParameters.clear();

		GLint numActiveUniforms = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

		GLint maxUniformNameLength = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
		std::vector<GLchar> uniformNameData(maxUniformNameLength);

		for (int uniform = 0; uniform < numActiveUniforms; uniform++) {
			GLint uniformSize = 0;
			GLenum uniformType = 0;
			GLsizei actualLength = 0;
			glGetActiveUniform(
				program,
				uniform,
				maxUniformNameLength,
				&actualLength,
				&uniformSize,
				&uniformType,
				&uniformNameData[0]);

			std::shared_ptr<ShaderParameter> parameter;
			if (!shader->TryGetShaderParameter(std::string(&uniformNameData[0]), parameter)) continue;

			const GLint uniformLocation = glGetUniformLocation(program, uniformNameData.data());
			uniforms.emplace_back(parameter);
			locationByParameter[parameter] = uniformLocation;
			typeByParameter[parameter] = uniformType;
			sizeByParameter[parameter] = uniformSize;
			parameter->OnParameterValueChangedEvent += Sharp::EventHandler::Bind(this, &OpenGLShader::HandleOnShaderParameterValueChangedEvent);
			dirtyParameters.emplace_back(parameter); // Initialize as dirty so all parameters are uploaded at least once
		}

		GLint numActiveAttributes = 0;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttributes);

		GLint maxAttributeNameLength = 0;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength);
		std::vector<GLchar> attributeNameData(maxAttributeNameLength);
		std::map<GLint, std::shared_ptr<ShaderParameter>> attributeByLocation {};

		for (int i = 0; i < numActiveAttributes; ++i) {
			GLint attributeSize = 0;
			GLenum attributeType = 0;
			GLsizei actualLength = 0;
			glGetActiveAttrib(program,
				i,
				maxAttributeNameLength,
				&actualLength,
				&attributeSize,
				&attributeType,
				&attributeNameData[0]);

			switch (attributeType) {
			case GL_FLOAT:
			case GL_INT:
			case GL_BOOL:
				attributeSize = 1;
				break;
			case GL_FLOAT_VEC2:
				attributeSize = 2;
				attributeType = GL_FLOAT;
				break;
			case GL_FLOAT_VEC3:
				attributeSize = 3;
				attributeType = GL_FLOAT;
				break;
			case GL_FLOAT_VEC4:
				attributeSize = 4;
				attributeType = GL_FLOAT;
				break;
			case GL_FLOAT_MAT4:
				attributeSize = 16;
				attributeType = GL_FLOAT;
				break;
			default:
				break;
			}

			std::shared_ptr<ShaderParameter> parameter;
			if (!shader->TryGetShaderParameter(std::string(&attributeNameData[0]), parameter)) continue;

			const GLint attributeLocation = glGetAttribLocation(program, attributeNameData.data());
			attributes.emplace_back(parameter);
			locationByParameter[parameter] = attributeLocation;
			typeByParameter[parameter] = attributeType;
			sizeByParameter[parameter] = attributeSize;
			attributeByLocation[attributeLocation] = parameter;
		}

		attributeBufferSize = 0;
		for (const std::shared_ptr<ShaderParameter>& attribute : attributes) {
			attributeBufferSize += sizeByParameter[attribute] * static_cast<GLint>(sizeof typeByParameter[attribute]);
			GLuint attributeOffset = 0;
			for(GLint i = locationByParameter[attribute] - 1; i >= 0; --i) {
				const std::shared_ptr<ShaderParameter>& previousAttribute = attributeByLocation[i];
				attributeOffset += sizeByParameter[previousAttribute] * sizeof typeByParameter[previousAttribute];
			}
			offsetByParameter[attribute] = attributeOffset;
		}
	}

	void OpenGLShader::ExtractParameters(const std::shared_ptr<Shader>& shader) const {
		GLint numActiveUniforms = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

		GLint maxUniformNameLength = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
		std::vector<GLchar> uniformNameData(maxUniformNameLength);

		for (int uniform = 0; uniform < numActiveUniforms; uniform++) {
			GLint uniformSize = 0;
			GLenum uniformType = 0;
			GLsizei actualLength = 0;
			glGetActiveUniform(
				program,
				uniform,
				maxUniformNameLength,
				&actualLength,
				&uniformSize,
				&uniformType,
				&uniformNameData[0]);

			ShaderParameterType shaderParameterType;
			if (!TryGetShaderParameterType(uniformType, shaderParameterType)) continue;
			shader->AddShaderParameter(std::string(&uniformNameData[0]), shaderParameterType, false);
		}

		GLint numActiveAttributes = 0;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttributes);

		GLint maxAttributeNameLength = 0;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength);
		std::vector<GLchar> attributeNameData(maxAttributeNameLength);

		for (int i = 0; i < numActiveAttributes; ++i) {
			GLint attributeSize = 0;
			GLenum attributeType = 0;
			GLsizei actualLength = 0;
			glGetActiveAttrib(program,
				i,
				maxAttributeNameLength,
				&actualLength,
				&attributeSize,
				&attributeType,
				&attributeNameData[0]);

			ShaderParameterType shaderParameterType;
			if (!TryGetShaderParameterType(attributeType, shaderParameterType)) continue;
			shader->AddShaderParameter(std::string(&attributeNameData[0]), shaderParameterType, false);
		}
	}

	void OpenGLShader::HandleOnShaderParameterValueChangedEvent(const std::shared_ptr<ShaderParameter>& shaderParameter) {
		if (std::ranges::find(dirtyParameters, shaderParameter) != dirtyParameters.end()) return;

		dirtyParameters.emplace_back(shaderParameter);
	}
}
#endif