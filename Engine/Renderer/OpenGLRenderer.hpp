#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Shader/OpenGLShader.hpp"

namespace Engine
{
	class ENGINE_API OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer() noexcept;
		explicit OpenGLRenderer(OpenGLShader* shader) noexcept;
		OpenGLRenderer(OpenGLRenderer const &other) = default;
		OpenGLRenderer(OpenGLRenderer &&other) noexcept = default;
		virtual ~OpenGLRenderer() noexcept;

		void RendererBegin(const glm::mat4x4& view, const glm::mat4x4& projection) override;
		void Render(const glm::mat4x4& modelMatrix, Model* model, const glm::vec4& mainColor = glm::vec4(1, 1, 1, 1)) override;
		void RendererEnd() override;
	private:

		OpenGLShader*	shader;
		ShaderParameter* projParam;
		ShaderParameter* modelParam;
		ShaderParameter* viewParam;
		ShaderParameter* textureParam;
		ShaderParameter* mainTextureColor;
		ShaderAttribute* positionAttrib;
		ShaderAttribute* normalAttrib;
		ShaderAttribute* textureAttrib;
	};
} // namespace Engine