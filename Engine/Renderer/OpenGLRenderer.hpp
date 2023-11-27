#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Engine/Engine.hpp"
#include "Engine/Mesh/OpenGLMesh.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Shader/OpenGLShader.hpp"

namespace Engine {
	class OpenGLTexture;

	class ENGINE_LOCAL OpenGLRenderer : public Renderer {
		friend std::shared_ptr<Renderer> Engine::GetRenderer() noexcept;

		std::shared_ptr<Window> window;
		std::map<xg::Guid, std::shared_ptr<OpenGLTexture>> loadedTextures {};
		std::map<xg::Guid, std::pair<int, std::shared_ptr<OpenGLShader>>> activeShaderUseCountByGuid {};
		std::map<xg::Guid, std::pair<int, std::shared_ptr<OpenGLMesh>>> activeMeshUseCountByGuid {};

	public:
		OpenGLRenderer();
		OpenGLRenderer(const OpenGLRenderer& other) = delete;
		OpenGLRenderer(OpenGLRenderer&& other) noexcept = delete;
		virtual ~OpenGLRenderer() override;

		OpenGLRenderer& operator=(const OpenGLRenderer& other) = delete;
		OpenGLRenderer& operator=(OpenGLRenderer&& other) noexcept = delete;

		virtual void InitializeTextureForRendering(std::shared_ptr<Texture> textureToInitialize) override;

		virtual void TryExtractShaderParameters(const std::shared_ptr<Shader>& shaderToLoadParametersFrom) override;

		[[nodiscard]] bool TryGetTextureForGuid(const xg::Guid& textureGuid, std::shared_ptr<OpenGLTexture>& texture);

	private:
		virtual void RendererBegin() override;

		virtual void RenderCommands(const std::shared_ptr<CameraComponent>& camera, const std::vector<RenderCommand>& commands) override;

		virtual void RendererEnd() override;

		virtual void HandleOnMeshRendererComponentMeshChangedEvent(std::shared_ptr<MeshRendererComponent> meshRendererComponent, std::shared_ptr<Mesh> oldMesh, std::shared_ptr<Mesh> newMesh) override;

		virtual void HandleOnRenderComponentMaterialChangedEvent(std::shared_ptr<RenderComponent> renderComponent, std::shared_ptr<Material> oldMaterial, std::shared_ptr<Material> newMaterial) override;

		void HandleOnMaterialShaderChangedEvent(const std::shared_ptr<Material>& material, const std::shared_ptr<Shader>& oldShader, const std::shared_ptr<Shader>& newShader);
	};
}
#endif