#pragma once

#include "Engine/api.hpp"
#include "Engine/Shader/Shader.hpp"
#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/EASTL-master/include/EASTL/map.h>
#include <ThirdParty/EASTL-master/include/EASTL/unique_ptr.h>

#define GLEW_STATIC
#include <ThirdParty/glew-2.1.0/include/GL/glew.h>

namespace Engine
{
	//class Shader;
	class Texture;
	class Color;

	///
	/// ShaderParameter is a representation of an shader parameter.
	/// It has a type and it will complain if the type declared in the
	/// shader program is different.
	///
	class ShaderParameter
	{
		friend class OpenGLShader;

	public:

		/// Check if this parameter object is valid
		bool IsValid() const { return _location != -1; }

		/// Gets the type of this parameter
		GLenum GetType() const { return _type; }

		/// Location of this parameter
		GLint GetLocation() const { return _location; }

		/// Set the value. Parameter should be float.
		void SetValue(float val);

		/// Set the value. Parameter should be int.
		void SetValue(int val);

		/// Set the value. Parameter should be bool.
		void SetValue(bool val);

		/// Set the value. Parameter should be Vector2.
		void SetValue(const glm::vec2& vec);

		/// Set the value. Parameter should be Vector3.
		void SetValue(const glm::vec3& vec);

		/// Set the value. Parameter should be Vector4.
		void SetValue(const glm::vec4& vec);

		/// Set the value. Parameter should be Color.
		void SetValue(const Color& color);

		/// Set the value. Parameter should be Matrix33.
		void SetValue(const glm::mat4x4& mtx, bool transpose = false);

		/// Set the value. Parameter should be Texture.
		void SetValue(Texture& texture);

	protected:

		/// The shader creates a parameter.
		ShaderParameter(Shader* shader, eastl::string name, GLenum type, GLint location, GLint sampler = -1)
			: _shader(shader)
			, _name(name)
			, _type(type)
			, _location(location)
			, _sampler(sampler)
		{}

		/// Constructor for an invalid ShaderParameter
		ShaderParameter() : _shader(nullptr), _name(""), _type(0), _location(-1), _sampler(-1) {}


		/// The shader can reset the parameter after a reload
		void Reset(Shader* shader, eastl::string name, GLenum type, GLint location, GLint sampler = -1)
		{
			this->_shader = shader;
			this->_type = type;
			this->_location = location;
			this->_name = name;
			this->_sampler = sampler;
		}

		/// The shader should invalidate when reloading a new shader file
		/// a some information can be old
		void Invalidate()
		{
			_shader = nullptr;
			_name = "";
			_type = 0;
			_location = -1;
			_sampler = -1;
		}

		/// The effect this parameter belongs to. Unused (beyond debugging)
		Shader*			_shader;

		/// Name as defined in the shader file.
		eastl::string		_name;

		/// Type as defined in the shader file.
		GLenum			_type;

		/// Location for the setting for this.
		GLint			_location;

		/// Only valid for type sampler (GL_SAMPLER_2D)
		GLint			_sampler;
	};


	///
	/// A vertex attribute
	///
	class ShaderAttribute
	{
		friend class OpenGLShader;

	public:

		/// Check if this parameter object is valid
		bool IsValid() const { return _location != -1; }

		/// Gets the type of this parameter
		GLenum GetType() const { return _type; }

		/// Location of this parameter
		GLint GetLocation() const { return _location; }

		/// Check documentation for glVertexAttribPointer
		void SetAttributePointer(
			GLint size,
			GLenum type,
			GLboolean normalized,
			GLsizei stride,
			const GLvoid * pointer);

		/// Check documentation for glVertexAttribPointer
		void DisableAttributePointer();

	protected:

		/// The shader creates a parameter.
		ShaderAttribute(Shader* shader, eastl::string name, GLenum type, GLint location) :
			_shader(shader),
			_name(name),
			_type(type),
			_location(location)
		{}

		/// Constructor for an invalid ShaderParameter
		ShaderAttribute() : _shader(nullptr), _name(""), _type(0), _location(-1) {}


		/// The shader can reset the parameter after a reload
		void Reset(Shader* shader, eastl::string name, GLenum type, GLint location)
		{
			this->_shader = shader;
			this->_type = type;
			this->_location = location;
			this->_name = name;
		}

		/// The shader should invalidate when reloading a new shader file
		/// as some information can be old
		void Invalidate()
		{
			_shader = nullptr;
			_name = "";
			_type = 0;
			_location = -1;
		}


	protected:
		/// The effect this parameter belongs to. Unused (beyond debugging)
		Shader*		_shader;

		/// Name as defined in the shader file.
		eastl::string  _name;

		/// Type as defined in the shader file.
		GLenum		_type;

		/// Location for the setting for this.
		GLint		_location;

	};

	///
	/// Shader is a class representing a compiled GPU program
	///
	class ENGINE_API OpenGLShader : public Shader
	{
	public:
		
		///Create a shader with vertex and fragment programs
		explicit OpenGLShader(	const eastl::string& vertexFileName,
								const eastl::string& fragmentFileName);

		///Create a shader with vertex, fragment and geometry programs
		explicit OpenGLShader(	const eastl::string& vertexFileName,
								const eastl::string& fragmentFileName,
								const eastl::string& geometryFileName);

		explicit OpenGLShader(const Shader& other) = delete;
		OpenGLShader(OpenGLShader &&other) = delete;

		/// Load with source (as oposed to files)
		bool LoadSource(const eastl::string& vertexShader,
						const eastl::string& fragmentShader,
						const eastl::string& geometryShader);

		/// Get active parameter by name. If the parameter is not present/active
		/// you will get an invalid one.
		ShaderParameter* GetParameter(const eastl::string& name);

		/// Get active attribute by name. If the attribute is not present/active
		/// you will get an invalid one.
		ShaderAttribute* GetAttribute(const eastl::string& name);

		/// Get the program in case you want to do everyting manually
		GLuint GetProgram() const;

		void Activate() override;
		void Deactivate() override;

	private:

		void LoadParameters();

		bool Validate();

		/// Store all the parameters
		eastl::map<eastl::string, eastl::unique_ptr<ShaderParameter>> parameters;

		/// Store all the attributes
		eastl::map<eastl::string, eastl::unique_ptr<ShaderAttribute>> attributes;

		/// GL id (name) of the compiled program
		GLuint program = 0;
	};
} //namespace Engine