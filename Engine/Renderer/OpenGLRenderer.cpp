#include "Engine/Utility/Defines.hpp"
#include "Engine/Window/Window.hpp"

#ifdef USING_OPENGL
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Renderer/imgui_impl_glfw_gl3.h"
#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Engine.hpp"

namespace Engine {
	OpenGLRenderer::OpenGLRenderer(const std::string& vertexShader, const std::string& fragmentShader)
	{
		ImGui_ImplGlfwGL3_Init(Window::Get()->GetGlfwWindow().get());

		this->shader = std::shared_ptr<OpenGLShader>(new OpenGLShader(vertexShader, fragmentShader));
		projParam = this->shader->GetParameter("u_projection");
		modelParam = this->shader->GetParameter("u_model");
		viewParam = this->shader->GetParameter("u_view");
		textureParam = this->shader->GetParameter("u_texture");
		mainTextureColor = this->shader->GetParameter("u_mainTextColor");

		positionAttribute = this->shader->GetAttribute("a_position");
		normalAttribute = this->shader->GetAttribute("a_normal");
		textureAttribute = this->shader->GetAttribute("a_texture");

	}

	OpenGLRenderer::~OpenGLRenderer()
	{
		ImGui_ImplGlfwGL3_Shutdown();
		window.reset(); // Keep window in memory so ImGUI can shutdown properly
	}

	void OpenGLRenderer::RendererBegin(const glm::mat4x4 & view, const glm::mat4x4 & projection)
	{
		ImGui_ImplGlfwGL3_NewFrame();
		window = Window::Get();
		glViewport(0, 0, window->GetDisplayWidth(), window->GetDisplayHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shader->Activate();

		viewParam->SetValue(view);
		projParam->SetValue(projection);
	}

	void OpenGLRenderer::Render(const glm::mat4x4& modelMatrix, const std::shared_ptr<Model> model, const glm::vec4& mainColor)
	{
		if (model == nullptr)
			return;

		modelParam->SetValue(modelMatrix);
		mainTextureColor->SetValue(mainColor);

		const std::vector<std::shared_ptr<Mesh>>& meshes = model->GetModelMeshes();
		for (const std::shared_ptr<Mesh>& mesh : meshes) {
			if (mesh == nullptr) continue;

			const GLuint vboBuffer = static_cast<GLuint>(mesh->GetVBO());
			const GLuint eboBuffer = static_cast<GLuint>(mesh->GetEBO());
			if (glIsBuffer(vboBuffer) != GL_TRUE)
				_ASSERT("The vertex buffer is not a valid buffer (VBO).");
			if (glIsBuffer(eboBuffer) != GL_TRUE)
				_ASSERT("The index buffer is not a valid buffer (EBO)");

			// Bind the buffers to the global state
			glBindBuffer(GL_ARRAY_BUFFER, vboBuffer);
			glCheckError();

			// Default to VBO values, the pointer addresses are interpreted as byte-offsets.
			const void* firstPosition = reinterpret_cast<const void*>(offsetof(Vertex, position));
			const void* firstNormal = reinterpret_cast<const void*>(offsetof(Vertex, normal));
			const void* firstTexture = reinterpret_cast<const void*>(offsetof(Vertex, texCoords));

			constexpr GLsizei stride = sizeof(Vertex);
			positionAttribute->SetAttributePointer(3, GL_FLOAT, GL_FALSE, stride, firstPosition);
			normalAttribute->SetAttributePointer(3, GL_FLOAT, GL_FALSE, stride, firstNormal);
			textureAttribute->SetAttributePointer(2, GL_FLOAT, GL_FALSE, stride, firstTexture);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboBuffer);
			glCheckError();

			const std::shared_ptr<Material> meshMaterial = model->GetMeshMaterial(mesh);
			if (meshMaterial->IsDiffuseLoaded())
				textureParam->SetValue(*meshMaterial->GetDiffuseTexture().lock());
			glCheckError();

			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->indices.size()), GL_UNSIGNED_INT, nullptr);
			glCheckError();

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glCheckError();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1);
			glCheckError();

			positionAttribute->DisableAttributePointer();
			normalAttribute->DisableAttributePointer();
			textureAttribute->DisableAttributePointer();
		}
	}

	void OpenGLRenderer::RendererEnd()
	{
		ImGui::Render();
		shader->Deactivate();
		glfwSwapBuffers(Window::Get()->GetGlfwWindow().get());
	}
} // namespace Engine
#endif // USING_OPENGL