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
	//class Shader;
	class Texture;
	class Color;

	/// <summary>
	/// ShaderParameter is a representation of an shader parameter.
	/// It has a type and it will complain if the type declared in the shader program is different.
	/// </summary>
	class ENGINE_API ShaderParameter
	{
		friend class OpenGLShader;

	public:

		/// <summary>
		/// Check if this parameter object is valid.
		/// </summary>
		/// <returns>Returns true of this Shader Parameter has been properly initialized. </returns>
		bool IsValid() const { return this != nullptr && _location != -1; }

		/// <summary>
		/// Gets the type of this parameter.
		/// </summary>
		/// <returns>Returns the type of this shader parameter as a GLenum.</returns>
		GLenum GetType() const { return _type; }

		/// <summary>
		/// The location of this parameter.
		/// </summary>
		/// <returns>Returns the location of this parameter as a GLint.</returns>
		GLint GetLocation() const { return _location; }
		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as a float.</param>
		void SetValue(float val) const;

		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as an int.</param>
		void SetValue(int val) const;

		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as a bool.</param>
		void SetValue(bool val) const;

		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as a vector 2.</param>
		void SetValue(const glm::vec2& vec) const;

		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as a vector 3.</param>
		void SetValue(const glm::vec3& vec) const;

		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as a vector 4.</param>
		void SetValue(const glm::vec4& vec) const;

		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as a color.</param>
		void SetValue(const Color& color);

		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as a matrix 4x4.</param>
		/// <param name="transpose">This method can optionally transpose the given matrix. False by default.</param>
		void SetValue(const glm::mat4x4& mtx, bool transpose = false) const;

		/// <summary>
		/// Set the value of this parameter.
		/// </summary>
		/// <param name="val">The value expected as a texture.</param>
		void SetValue(Texture& texture_) const;

	protected:

		/// <summary>
		/// The constructor of a shader parameter, called by OpenGLShader.
		/// </summary>
		/// <param name="shader">A reference to the current shader.</param>
		/// <param name="name">The name of the parameter you want to load in.</param>
		/// <param name="type">The type of the parameter you want to load in.</param>
		/// <param name="location">The location of the parameter you want to load in.</param>
		/// <param name="sampler">The sampler of this parameter. -1 by default.</param>
		ShaderParameter(OpenGLShader* shader, std::string name, GLenum type, GLint location, GLint sampler = -1)
			: _shader(shader)
			, _name(name)
			, _type(type)
			, _location(location)
			, _sampler(sampler)
		{}

		/// <summary>
		/// A constructor for an invalid ShaderParameter.
		/// </summary>
		ShaderParameter() : _shader(nullptr), _name(""), _type(0), _location(-1), _sampler(-1) {}

		/// <summary>
		/// The shader is able to reset a shader parameter after a reload.
		/// </summary>
		/// <param name="shader">A reference to the current shader.</param>
		/// <param name="name">The name of the parameter you want to load in.</param>
		/// <param name="type">The type of the parameter you want to load in.</param>
		/// <param name="location">The location of the parameter you want to load in.</param>
		/// <param name="sampler">The sampler of this parameter. -1 by default.</param>
		void Reset(OpenGLShader* shader, std::string name, GLenum type, GLint location, GLint sampler = -1)
		{
			this->_shader = shader;
			this->_type = type;
			this->_location = location;
			this->_name = name;
			this->_sampler = sampler;
		}

		/// <summary>
		/// The shader should invalidate this shader parameter when reloading a new shader file. Otherwise some of the information can be old.
		/// </summary>
		void Invalidate()
		{
			_shader = nullptr;
			_name = "";
			_type = 0;
			_location = -1;
			_sampler = -1;
		}

		/// <summary>
		/// A reference to the shader this shader parameter belongs to.
		/// </summary>
		OpenGLShader*	_shader;

		/// <summary>
		/// Name of the shader parameter as defined in the shader file.
		/// </summary>
		std::string		_name;

		/// <summary>
		/// The type of the shader parameter as defined in the shader file.
		/// </summary>
		GLenum			_type;

		/// <summary>
		/// The location of this shader parameter.
		/// </summary>
		GLint			_location;

		/// <summary>
		/// The sampler of this shader parameter. NOTE: Only valid for the type GL_SAMPLER_2D/
		/// </summary>
		GLint			_sampler;
	};


	/// <summary>
	/// ShaderAttrixbute is a representation of a vertex shader attribute.
	/// </summary>
	class ENGINE_API ShaderAttribute
	{
		friend class OpenGLShader;

	public:

		/// <summary>
		/// Check if this parameter object is valid.
		/// </summary>
		/// <returns>Returns true of this Shader Parameter has been properly initialized. </returns>
		bool IsValid() const { return this != nullptr && _location != -1; }

		/// <summary>
		/// Gets the type of this parameter.
		/// </summary>
		/// <returns>Returns the type of this shader parameter as a GLenum.</returns>
		GLenum GetType() const { return _type; }

		/// <summary>
		/// The location of this parameter.
		/// </summary>
		/// <returns>Returns the location of this parameter as a GLint.</returns>
		GLint GetLocation() const { return _location; }

		/// Check documentation for glVertexAttribPointer
		/// <summary>
		/// This method allows you to set the value of your shader attribute. NOTE: see this link for more information: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
		/// </summary>
		/// <param name="size">The size of the shader attribute.</param>
		/// <param name="type">The type of the shader attribute.</param>
		/// <param name="normalized">This bool defines if this value should be normalized before it's sent to the GPU.</param>
		/// <param name="stride">The stride of the shader attribute.</param>
		/// <param name="pointer">The pointer of the shader attribute.</param>
		void SetAttributePointer(
			GLint size,
			GLenum type,
			GLboolean normalized,
			GLsizei stride,
			const GLvoid * pointer) const;

		/// <summary>
		/// This method allows you to disable the shader attribute. 
		/// </summary>
		void DisableAttributePointer() const;

	protected:

		/// The shader creates a parameter.
		/// <summary>
		/// The constructor of a shader attribute, called by OpenGLShader.
		/// </summary>
		/// <param name="shader">A reference to the current shader.</param>
		/// <param name="name">The name of the attribute you want to load in.</param>
		/// <param name="type">The type of the attribute you want to load in.</param>
		/// <param name="location">The location of the attribute you want to load in.</param>
		ShaderAttribute(OpenGLShader* shader, std::string name, GLenum type, GLint location) :
			_shader(shader),
			_name(name),
			_type(type),
			_location(location)
		{}

		/// <summary>
		/// A constructor for an invalid ShaderAttribute.
		/// </summary>
		ShaderAttribute() : _shader(nullptr), _name(""), _type(0), _location(-1) {}


		/// <summary>
		/// The shader is able to reset a shader attribute after a reload.
		/// </summary>
		/// <param name="shader">A reference to the current shader.</param>
		/// <param name="name">The name of the attribute you want to load in.</param>
		/// <param name="type">The type of the attribute you want to load in.</param>
		/// <param name="location">The location of the attribute you want to load in.</param>
		void Reset(OpenGLShader* shader, std::string name, GLenum type, GLint location)
		{
			this->_shader = shader;
			this->_type = type;
			this->_location = location;
			this->_name = name;
		}

		/// <summary>
		/// The shader should invalidate this shader parameter when reloading a new shader file. Otherwise some of the information can be old.
		/// </summary>
		void Invalidate()
		{
			_shader = nullptr;
			_name = "";
			_type = 0;
			_location = -1;
		}


	protected:
		/// <summary>
		/// A reference to the shader this shader attribute belongs to.
		/// </summary>
		OpenGLShader*	_shader;

		/// <summary>
		/// Name of the shader attribute as defined in the shader file.
		/// </summary>
		std::string  _name;

		/// <summary>
		/// The type of the shader attribute as defined in the shader file.
		/// </summary>
		GLenum		_type;

		/// <summary>
		/// The location of this shader attribute.
		/// </summary>
		GLint		_location;
	};

	/// <summary>
	/// This object is used to store information regarding the OpenGLShader.
	/// </summary>
	class ENGINE_API OpenGLShader : public Shader
	{
	private:

	private:
		friend class OpenGLRenderer;

		///Create a shader with vertex and fragment programs
		/// <summary>
		/// Constructor that allows you to create a shader instande with a vertex file and fragment file.
		/// </summary>
		/// <param name="vertexFileName">The vertex shader file you want to load.
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		/// <param name="fragmentFileName">The fragment shader file you want to load. 
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		explicit OpenGLShader(const std::string& vertexFileName,
			const std::string& fragmentFileName);

		///Create a shader with vertex and fragment programs
		/// <summary>
		/// Constructor that allows you to create a shader instande with a vertex file and fragment file.
		/// </summary>
		/// <param name="vertexFileName">The vertex shader file you want to load.
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		/// <param name="fragmentFileName">The fragment shader file you want to load. 
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		/// <param name="geometryFileName">The geometry shader file you want to load. This value is optional.
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		explicit OpenGLShader(const std::string& vertexFileName,
			const std::string& fragmentFileName,
			const std::string& geometryFileName);

		explicit OpenGLShader(const Shader& other) = delete;
		OpenGLShader(OpenGLShader &&other) = delete;

	public:
		///Create a shader with vertex and fragment programs
		/// <summary>
		/// Constructor that allows you to create a shader instande with a vertex file and fragment file.
		/// </summary>
		/// <param name="vertexFileName">The vertex shader file you want to load.
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		/// <param name="fragmentFileName">The fragment shader file you want to load. 
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		/// <param name="geometryFileName">The geometry shader file you want to load. This value is optional.
		/// NOTE: Only use the shader name + extension. The shader file needs to be located under 'Resources/Shaders/'. It can be in a subfolder of 'Resources/Shaders/'.</param>
		/// <returns>Returns true if the shader files have succesfully been loaded.</returns>
		bool LoadSource(const std::string& vertexShader,
			const std::string& fragmentShader,
			const std::string& geometryShader);

		/// <summary>
		/// This method will allow you to get a parameter by its name.
		/// </summary>
		/// <param name="name">The name of the parameter you want to get.</param>
		/// <returns>Returns the parameter if it has been loaded. Otherwise it'll return an expired weak pointer.</returns>
		std::shared_ptr<ShaderParameter> GetParameter(const std::string& name);

		/// <summary>
		/// This method will allow you to get a attribute by its name.
		/// </summary>
		/// <param name="name">The name of the attribute you want to get.</param>
		/// <returns>Returns the attribute if it has been loaded. Otherwise it'll return an expired weak pointer.</returns>
		std::shared_ptr<ShaderAttribute> GetAttribute(const std::string& name);

		/// <summary>
		/// This method allows you to get the program of this shader.
		/// </summary>
		/// <returns>Returns the program as a GLuint.</returns>
		GLuint GetProgram() const;

		/// <summary>
		/// Call this method before you start rendering.
		/// </summary>
		void Activate() override;
		/// <summary>
		/// Call this method after you've finished rendering.
		/// </summary>
		void Deactivate() override;

	private:

		void LoadParameters();

		bool Validate() const;

		/// Store all the parameters
		std::map<std::string, std::shared_ptr<ShaderParameter>> parameters;

		/// Store all the attributes
		std::map<std::string, std::shared_ptr<ShaderAttribute>> attributes;

		/// GL id (name) of the compiled program
		GLuint program = 0;
	};
} //namespace Engine
#endif