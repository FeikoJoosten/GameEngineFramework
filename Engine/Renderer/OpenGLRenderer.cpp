#include "Engine/Utility/Defines.hpp"

#ifdef USING_OPENGL
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Renderer/imgui_impl_glfw_gl3.h"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/engine.hpp"

namespace Engine {
	OpenGLRenderer::OpenGLRenderer(eastl::string vertexShader, eastl::string fragmentShader) noexcept
	{
		this->shader = eastl::shared_ptr<OpenGLShader>(new OpenGLShader(vertexShader, fragmentShader));
		projParam = this->shader->GetParameter("u_projection");
		modelParam = this->shader->GetParameter("u_model");
		viewParam = this->shader->GetParameter("u_view");
		textureParam = this->shader->GetParameter("u_texture");
		mainTextureColor = this->shader->GetParameter("u_mainTextColor");

		positionAttrib = this->shader->GetAttribute("a_position");
		normalAttrib = this->shader->GetAttribute("a_normal");
		textureAttrib = this->shader->GetAttribute("a_texture");

		ImGui_ImplGlfwGL3_Init(Engine::GetEngine().lock()->GetWindow().lock()->GetGLFWWindow().lock().get());
	}

	OpenGLRenderer::~OpenGLRenderer() noexcept
	{
		ImGui_ImplGlfwGL3_Shutdown();
	}

	void OpenGLRenderer::RendererBegin(const glm::mat4x4 & view, const glm::mat4x4 & projection)
	{
		ImGui_ImplGlfwGL3_NewFrame();
		glViewport(0, 0, Engine::GetEngine().lock()->GetWindow().lock()->GetDisplayWidth(), Engine::GetEngine().lock()->GetWindow().lock()->GetDisplayHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shader->Activate();

		viewParam.lock()->SetValue(view);
		projParam.lock()->SetValue(projection);
	}

	void OpenGLRenderer::Render(const glm::mat4x4& modelMatrix, eastl::shared_ptr<Model> model, const glm::vec4& mainColor)
	{
		if (model == nullptr)
			return;

		modelParam.lock()->SetValue(modelMatrix);
		mainTextureColor.lock()->SetValue(mainColor);

		eastl::vector<eastl::shared_ptr<Mesh>>& meshes = model->GetModelMeshes();
		for (size_t i = 0, size = meshes.size(); i < size; ++i)
		{
			if (glIsBuffer(GLuint(meshes[i]->GetVBO())) != GL_TRUE)
			{
				_ASSERT("The vertex buffer is not a valid buffer (VBO).");
			}
			if (glIsBuffer(GLuint(meshes[i]->GetEBO())) != GL_TRUE)
			{
				_ASSERT("The index buffer is not a valid buffer (EBO)");
			}

			// Bind the buffers to the global state
			glBindBuffer(GL_ARRAY_BUFFER, GLuint(meshes[i]->GetVBO()));
			glGetError();

			// Default to VBO values, the pointer addresses are interpreted as byte-offsets.
			const void* firstPosition = reinterpret_cast<const void*>(offsetof(Vertex, position));
			const void* firstNormal = reinterpret_cast<const void*>(offsetof(Vertex, normal));
			const void* firstTexture = reinterpret_cast<const void*>(offsetof(Vertex, texCoords));

			GLsizei stride = sizeof(Vertex);
			positionAttrib.lock()->SetAttributePointer(3, GL_FLOAT, GL_FALSE, stride, firstPosition);
			normalAttrib.lock()->SetAttributePointer(3, GL_FLOAT, GL_FALSE, stride, firstNormal);
			textureAttrib.lock()->SetAttributePointer(2, GL_FLOAT, GL_FALSE, stride, firstTexture);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLuint(meshes[i]->GetEBO()));
			glGetError();

			if (model->GetMeshMaterial(meshes[i])->IsDiffuseLoaded())
			{
				textureParam.lock()->SetValue(*model->GetMeshMaterial(meshes[i])->GetDiffuseTexture());
			}
			glGetError();

			glDrawElements(GL_TRIANGLES, GLsizei(meshes[i]->indices.size()), GL_UNSIGNED_INT, reinterpret_cast<const void*>(0));
			glGetError();

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glGetError();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1);
			glGetError();

			positionAttrib.lock()->DisableAttributePointer();
			normalAttrib.lock()->DisableAttributePointer();
			textureAttrib.lock()->DisableAttributePointer();
		}
	}

	void OpenGLRenderer::RendererEnd()
	{
		ImGui::Render();
		shader->Deactivate();
		Engine::GetEngine().lock()->GetWindow().lock()->SwapBuffers();
	}
} // namespace Engine
#endif // USING_OPENGL