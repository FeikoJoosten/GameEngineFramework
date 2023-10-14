#include "Engine/Utility/Defines.hpp"

#ifdef USING_OPENGL
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Model/Model.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Renderer/imgui_impl_glfw_gl3.h"
#include "Engine/Window/Window.hpp"

namespace Engine {
	glm::mat4 viewProjection;

	OpenGLRenderer::OpenGLRenderer(const std::string& vertexShader, const std::string& fragmentShader)
	{
		window = Window::Get();
		ImGui_ImplGlfwGL3_Init(window->GetGlfwWindow());
		window->OnWindowShutdownRequestedEvent += Sharp::EventHandler::Bind(this, &OpenGLRenderer::HandleOnWindowShutdownRequestedEvent);

		this->shader = std::shared_ptr<OpenGLShader>(new OpenGLShader(vertexShader, fragmentShader));
		modelViewProjectionParam = this->shader->GetParameter("u_mvp");
		textureParam = this->shader->GetParameter("u_texture");
		mainTextureColor = this->shader->GetParameter("u_mainTextColor");

		positionAttribute = this->shader->GetAttribute("a_position");
		normalAttribute = this->shader->GetAttribute("a_normal");
		textureAttribute = this->shader->GetAttribute("a_texture");
	}

	void OpenGLRenderer::RendererBegin(const glm::mat4x4 & view, const glm::mat4x4 & projection)
	{
		ImGui_ImplGlfwGL3_NewFrame();
		glViewport(0, 0, window->GetDisplayWidth(), window->GetDisplayHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shader->Activate();

		viewProjection = projection * view;
	}

	void OpenGLRenderer::Render(const glm::mat4x4& modelMatrix, const std::shared_ptr<Model> model)
	{
		if (model == nullptr)
			return;

		//modelViewProjectionParam->SetValue(viewProjection * modelMatrix);
		//mainTextureColor->SetValue(mainColor);

		//const std::vector<std::shared_ptr<Mesh>>& meshes = model->GetModelMeshes();
		//for (const std::shared_ptr<Mesh>& mesh : meshes) {
		//	if (mesh == nullptr) continue;

		//	const GLuint vboBuffer = static_cast<GLuint>(mesh->GetVBO());
		//	const GLuint eboBuffer = static_cast<GLuint>(mesh->GetEBO());
		//	if (glIsBuffer(vboBuffer) != GL_TRUE)
		//		_ASSERT("The vertex buffer is not a valid buffer (VBO).");
		//	if (glIsBuffer(eboBuffer) != GL_TRUE)
		//		_ASSERT("The index buffer is not a valid buffer (EBO)");

		//	// Bind the buffers to the global state
		//	glBindBuffer(GL_ARRAY_BUFFER, vboBuffer);
		//	glCheckError();
		//	
		//	positionAttribute->SetAttributePointer(GL_FALSE, 0, nullptr);
		//	normalAttribute->SetAttributePointer(GL_FALSE, 0, nullptr);
		//	textureAttribute->SetAttributePointer(GL_FALSE, 0, nullptr);

		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBuffer);
		//	glCheckError();

		//	const std::shared_ptr<Material> meshMaterial = model->GetMeshMaterial(mesh);
		//	textureParam->SetValue(*meshMaterial->GetDiffuseTexture().lock()); // Defaults to missing texture if no diffuse is loaded
		//	glCheckError();

		//	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->GetIndices().size()), GL_UNSIGNED_INT, nullptr);
		//	glCheckError();

		//	glBindBuffer(GL_ARRAY_BUFFER, 0);
		//	glCheckError();
		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1);
		//	glCheckError();

		//	positionAttribute->DisableAttributePointer();
		//	normalAttribute->DisableAttributePointer();
		//	textureAttribute->DisableAttributePointer();
		//}
	}

	void OpenGLRenderer::RendererEnd()
	{
		ImGui::Render();
		shader->Deactivate();
		glfwSwapBuffers(Window::Get()->GetGlfwWindow());
	}

	void OpenGLRenderer::HandleOnWindowShutdownRequestedEvent(const std::shared_ptr<Window> windowPtr) {
		windowPtr->OnWindowShutdownRequestedEvent -= Sharp::EventHandler::Bind(this, &OpenGLRenderer::HandleOnWindowShutdownRequestedEvent);
		ImGui_ImplGlfwGL3_Shutdown();
	}
}
#endif