#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Engine/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Shader/OpenGLShader.hpp"

namespace Engine {
	/// <summary>
	/// This is the OpenGL renderer. Allows you to render using OpenGL. NOTE: Only the Engine is allowed to create this object.
	/// </summary>
	class ENGINE_API OpenGLRenderer : public Renderer {
		friend std::shared_ptr<Renderer> Engine::GetRenderer() noexcept;

		// TODO: Remove shader paths from OpenGLRenderer and move to a material + shader based approach
		OpenGLRenderer(const std::string& vertexShader, const std::string& fragmentShader);

	public:
		OpenGLRenderer() = delete;
		OpenGLRenderer(const OpenGLRenderer& other) = delete;
		OpenGLRenderer(OpenGLRenderer&& other) noexcept = delete;
		virtual ~OpenGLRenderer() override = default;

		OpenGLRenderer& operator=(const OpenGLRenderer& other) = delete;
		OpenGLRenderer& operator=(OpenGLRenderer&& other) noexcept = delete;

	private:
		/// <summary>
		/// This method is used to initialize the renderer for the current frame.
		/// </summary>
		/// <param name="view">The view matrix used for this frame.</param>
		/// <param name="projection">The projection matrix used for this frame.</param>
		virtual void RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection) override;
		/// <summary>
		/// This method is used to send draw data to the GPU.
		/// </summary>
		/// <param name="modelMatrix">The model matrix of the object you want to draw.</param>
		/// <param name="model">The model to render.</param>
		/// <param name="mainColor">The color you want to render your model in. By default this is set to white.</param>
		virtual void Render(const glm::mat4x4& modelMatrix, std::shared_ptr<Model> model, const glm::vec4& mainColor = glm::vec4(1, 1, 1, 1)) override;
		/// <summary>
		/// This method is used to reset the current frame, so it's ready for the next frame.
		/// </summary>
		virtual void RendererEnd() override;

		void HandleOnWindowShutdownRequestedEvent(std::shared_ptr<Window> windowPtr);

		std::shared_ptr<OpenGLShader> shader;
		std::shared_ptr<Window> window;
		std::shared_ptr<ShaderParameter> modelViewProjectionParam;
		std::shared_ptr<ShaderParameter> textureParam;
		std::shared_ptr<ShaderParameter> mainTextureColor;
		std::shared_ptr<ShaderAttribute> positionAttribute;
		std::shared_ptr<ShaderAttribute> normalAttribute;
		std::shared_ptr<ShaderAttribute> textureAttribute;
	};
} // namespace Engine
#endif // USING_OPENGL