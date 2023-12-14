#include "Engine/Platform/OpenGL/Renderer/OpenGLRenderer.hpp"
#include "Engine/Utility/Defines.hpp"

#ifdef USING_OPENGL
#include "Engine/Components/CameraComponent.hpp"
#include "Engine/Components/RenderComponent.hpp"
#include "Engine/Material/Material.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Platform/OpenGL/Renderer/imgui_impl_glfw_gl3.h"
#include "Engine/Platform/OpenGL/Renderer/OpenGLUtility.hpp"
#include "Engine/Platform/OpenGL/Texture/OpenGLTexture.hpp"
#include "Engine/Renderer/RenderCommand.hpp"
#include "Engine/Shader/ShaderConstants.hpp"
#include "Engine/Texture/Texture.hpp"
#include "Engine/Window/Window.hpp"

namespace Engine {
	bool isRendering;

	std::map<std::shared_ptr<Shader>, std::shared_ptr<ShaderParameter>> modelViewProjectionShaderParameterByShader = {};
	std::map<std::shared_ptr<Shader>, std::shared_ptr<ShaderParameter>> lightDirectionShaderParameterByShader = {};

	OpenGLRenderer::OpenGLRenderer() : Renderer() {
		window = Window::Get();
		ImGui_ImplGlfwGL3_Init(window->GetGlfwWindow());
	}

	OpenGLRenderer::~OpenGLRenderer() {
		for (const std::shared_ptr<RenderComponent>& renderComponent : activeRenderComponents) {
			const std::shared_ptr<Material>& material = renderComponent->GetMaterial();
			if (!material) continue;

			material->OnShaderChangedEvent -= Sharp::EventHandler::Bind(this, &OpenGLRenderer::HandleOnMaterialShaderChangedEvent);
		}
	}

	void OpenGLRenderer::InitializeTextureForRendering(std::shared_ptr<Texture> textureToInitialize) {
		const xg::Guid textureGuid = textureToInitialize->GetGuid();
		if (loadedTextures.contains(textureGuid)) return; // Prevent loading textures twice

		loadedTextures[textureGuid] = std::make_shared<OpenGLTexture>(textureToInitialize);
	}

	void OpenGLRenderer::TryExtractShaderParameters(const std::shared_ptr<Shader>& shaderToLoadParametersFrom) {
		const OpenGLShader* shader = new OpenGLShader(shaderToLoadParametersFrom);
		shader->ExtractParameters(shaderToLoadParametersFrom);
		delete shader;
	}

	bool OpenGLRenderer::TryGetTextureForGuid(const xg::Guid& textureGuid, std::shared_ptr<OpenGLTexture>& texture) {
		const std::map<xg::Guid, std::shared_ptr<OpenGLTexture>>::iterator& textureIterator = loadedTextures.find(textureGuid);

		if (textureIterator == loadedTextures.end()) return false;

		texture = textureIterator->second;
		return true;
	}

	void OpenGLRenderer::RendererBegin() {
		isRendering = true;
		ImGui_ImplGlfwGL3_NewFrame();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRenderer::RenderCommands(const std::shared_ptr<CameraComponent>& camera,	const std::vector<RenderCommand>& commands) {
		const glm::mat4 projectionView = camera->GetProjectionView();

		// TODO: Add sorting based on same material
		for (const RenderCommand& command : commands) {
			const std::shared_ptr<Material>& material = command.GetMaterial();
			if (!material) continue;

			const std::shared_ptr<Shader>& shader = material->GetShader();
			if (!shader) continue;

			const std::shared_ptr<Mesh>& mesh = command.GetMesh();
			if (!mesh) continue;

			// Verify if the shader is properly loaded as an open gl shader
			const std::map<xg::Guid, std::pair<int, std::shared_ptr<OpenGLShader>>>::iterator& shaderIterator = activeShaderUseCountByGuid.find(shader->GetGuid());
			if (shaderIterator == activeShaderUseCountByGuid.end()) 
				continue;

			const std::shared_ptr<OpenGLShader> openGlShader = shaderIterator->second.second;
			openGlShader->Activate();

			std::shared_ptr<ShaderParameter> modelViewProjectionShaderParameter;
			if (const std::map<std::shared_ptr<Shader>, std::shared_ptr<ShaderParameter>>::iterator& modelViewProjectionShaderParameterIterator = modelViewProjectionShaderParameterByShader.find(shader); modelViewProjectionShaderParameterIterator == modelViewProjectionShaderParameterByShader.end()) {
				if (shader->TryGetShaderParameter(ShaderConstants::MODEL_VIEW_PROJECTION, modelViewProjectionShaderParameter))
					modelViewProjectionShaderParameterByShader[shader] = modelViewProjectionShaderParameter;
			} else modelViewProjectionShaderParameter = modelViewProjectionShaderParameterIterator->second;

			// Could potentially move this to the GPU and upload the projection view separate from the full mvp
			if (modelViewProjectionShaderParameter) modelViewProjectionShaderParameter->SetValue(projectionView * command.GetModelMatrix());

			// This only supports one light source atm, but the base shader doesn't support more than 1 light anyways
			std::shared_ptr<ShaderParameter> lightDirectionShaderParameter;
			if (const std::map<std::shared_ptr<Shader>, std::shared_ptr<ShaderParameter>>::iterator& lightDirectionShaderParameterIterator = lightDirectionShaderParameterByShader.find(shader); lightDirectionShaderParameterIterator == lightDirectionShaderParameterByShader.end()) {
				if (shader->TryGetShaderParameter(ShaderConstants::LIGHT_DIRECTION, lightDirectionShaderParameter))
					lightDirectionShaderParameterByShader[shader] = lightDirectionShaderParameter;
			} else lightDirectionShaderParameter = lightDirectionShaderParameterIterator->second;

			if (lightDirectionShaderParameter) {
				if (const std::shared_ptr<TransformComponent>& cameraTransformComponent = camera->GetComponent<TransformComponent>())
					lightDirectionShaderParameter->SetValue(cameraTransformComponent->GetForward());
			}

			openGlShader->ProcessDirtyShaderParameters();
			
			if (const std::map<xg::Guid, std::pair<int, std::shared_ptr<OpenGLMesh>>>::iterator& iterator =	activeMeshUseCountByGuid.find(mesh->GetGuid()); iterator != activeMeshUseCountByGuid.end()) {
				const std::shared_ptr<OpenGLMesh>& openGlMesh = iterator->second.second;
				glBindBuffer(GL_ARRAY_BUFFER, openGlMesh->vbo);
				glCheckError();

				openGlShader->EnableAttributes();

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGlMesh->ebo);
				glCheckError();

				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->GetIndices().size()), GL_UNSIGNED_INT, nullptr);
				glCheckError();

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glCheckError();
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1); // Should this be 0 as well?
				glCheckError();

				openGlShader->DisableAttributes();
			}
		}
	}

	void OpenGLRenderer::RendererEnd() {
		if (window->ShouldClose()) {
			if (isRendering) ImGui_ImplGlfwGL3_Shutdown();
			return;
		}
		ImGui::Render();
		glfwSwapBuffers(Window::Get()->GetGlfwWindow());
		isRendering = false;
	}

	void OpenGLRenderer::HandleOnMeshRendererComponentMeshChangedEvent(std::shared_ptr<MeshRendererComponent> meshRendererComponent, std::shared_ptr<Mesh> oldMesh,	std::shared_ptr<Mesh> newMesh) {
		Renderer::HandleOnMeshRendererComponentMeshChangedEvent(meshRendererComponent, oldMesh, newMesh);

		if (oldMesh) {
			const xg::Guid& oldMeshGuid = oldMesh->GetGuid();
			if (const std::map<xg::Guid, std::pair<int, std::shared_ptr<OpenGLMesh>>>::iterator& iterator = activeMeshUseCountByGuid.find(oldMeshGuid); iterator != activeMeshUseCountByGuid.end()) {
				if (--iterator->second.first <= 0)
					activeMeshUseCountByGuid.erase(oldMeshGuid);
				else
					activeMeshUseCountByGuid[oldMeshGuid] = iterator->second;
			}
		}

		if (!newMesh) return;

		const xg::Guid& newMeshGuid = newMesh->GetGuid();
		if (const std::map<xg::Guid, std::pair<int, std::shared_ptr<OpenGLMesh>>>::iterator& iterator = activeMeshUseCountByGuid.find(newMeshGuid); iterator == activeMeshUseCountByGuid.end()) {
			std::shared_ptr<OpenGLMesh> mesh = std::shared_ptr<OpenGLMesh>(new OpenGLMesh(newMesh));
			activeMeshUseCountByGuid[newMeshGuid] = { 1, mesh };
		} else
			activeMeshUseCountByGuid[newMeshGuid] = { iterator->second.first + 1, iterator->second.second };
	}

	void OpenGLRenderer::HandleOnRenderComponentMaterialChangedEvent(std::shared_ptr<RenderComponent> renderComponent, std::shared_ptr<Material> oldMaterial, std::shared_ptr<Material> newMaterial) {
		Renderer::HandleOnRenderComponentMaterialChangedEvent(renderComponent, oldMaterial, newMaterial);

		if (oldMaterial) {
			oldMaterial->OnShaderChangedEvent -= Sharp::EventHandler::Bind(this, &OpenGLRenderer::HandleOnMaterialShaderChangedEvent);
			HandleOnMaterialShaderChangedEvent(oldMaterial, oldMaterial->GetShader(), nullptr);
		}
		if (!newMaterial) return;

		newMaterial->OnShaderChangedEvent += Sharp::EventHandler::Bind(this, &OpenGLRenderer::HandleOnMaterialShaderChangedEvent);
		HandleOnMaterialShaderChangedEvent(newMaterial, nullptr, newMaterial->GetShader());
	}

	void OpenGLRenderer::HandleOnMaterialShaderChangedEvent([[maybe_unused]] const std::shared_ptr<Material>& material, const std::shared_ptr<Shader>& oldShader, const std::shared_ptr<Shader>& newShader) {
		if (oldShader) {
			const xg::Guid& oldShaderGuid = oldShader->GetGuid();
			if (const std::map<xg::Guid, std::pair<int, std::shared_ptr<OpenGLShader>>>::iterator& iterator = activeShaderUseCountByGuid.find(oldShaderGuid); iterator != activeShaderUseCountByGuid.end()) {
				if (--iterator->second.first <= 0)
					activeShaderUseCountByGuid.erase(oldShaderGuid);
				else
					activeShaderUseCountByGuid[oldShaderGuid] = iterator->second;
			}
		}

		if (!newShader) return;

		const xg::Guid& newShaderGuid = newShader->GetGuid();
		if (const std::map<xg::Guid, std::pair<int, std::shared_ptr<OpenGLShader>>>::iterator& iterator = activeShaderUseCountByGuid.find(newShaderGuid); iterator == activeShaderUseCountByGuid.end()) {
			std::shared_ptr<OpenGLShader> shader = std::shared_ptr<OpenGLShader>(new OpenGLShader(newShader));
			shader->LoadParameters(newShader);
			for (const std::shared_ptr<ShaderParameter>& parameter : newShader->GetShaderParameters()) {
				if (parameter->GetType() != ShaderParameterType::Texture) continue;

				// TODO: Add support for dynamic changing of this variable
				const std::shared_ptr<Texture>& texture = parameter->GetTextureValue();
				if (!texture) continue;

				InitializeTextureForRendering(texture);
			}
			activeShaderUseCountByGuid[newShaderGuid] = { 1, shader };
		} else
			activeShaderUseCountByGuid[newShaderGuid] = { iterator->second.first + 1, iterator->second.second };
	}
}
#endif