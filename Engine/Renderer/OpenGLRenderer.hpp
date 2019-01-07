#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Shader/OpenGLShader.hpp"

namespace Engine
{
	/// <summary>
	/// This is the OpenGL renderer. Allows you to render using OpenGL. NOTE: Only the Engine is allowed to create this object.
	/// </summary>
	class ENGINE_API OpenGLRenderer : public Renderer
	{
	public:
		friend class Engine;

		OpenGLRenderer() = default;
		explicit OpenGLRenderer(eastl::string vertexShader, eastl::string fragmentShader) noexcept;
		OpenGLRenderer(OpenGLRenderer const &other) = default;
		OpenGLRenderer(OpenGLRenderer &&other) noexcept = default;
		virtual ~OpenGLRenderer() noexcept;

		/// <summary>
		/// This method is used to initialize the renderer for the current frame.
		/// </summary>
		/// <param name="view">The view matrix used for this frame.</param>
		/// <param name="projection">The projection matrix used for this frame.</param>
		void RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection) override;
		/// <summary>
		/// This method is used to send draw data to the GPU.
		/// </summary>
		/// <param name="modelMatrix">The model matrix of the object you want to draw.</param>
		/// <param name="model">The model to render.</param>
		/// <param name="mainColor">The color you want to render your model in. By default this is set to white.</param>
		void Render(const glm::mat4x4& modelMatrix, eastl::shared_ptr<Model> model, const glm::vec4& mainColor = glm::vec4(1, 1, 1, 1)) override;
		/// <summary>
		/// This method is used to reset the current frame, so it's ready for the next frame.
		/// </summary>
		void RendererEnd() override;
	private:

		eastl::shared_ptr<OpenGLShader> shader;
		eastl::weak_ptr<ShaderParameter> projParam;
		eastl::weak_ptr<ShaderParameter> modelParam;
		eastl::weak_ptr<ShaderParameter> viewParam;
		eastl::weak_ptr<ShaderParameter> textureParam;
		eastl::weak_ptr<ShaderParameter> mainTextureColor;
		eastl::weak_ptr<ShaderAttribute> positionAttrib;
		eastl::weak_ptr<ShaderAttribute> normalAttrib;
		eastl::weak_ptr<ShaderAttribute> textureAttrib;
	};
} // namespace Engine
#endif // USING_OPENGL