#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/imgui_impl_glfw_gl3.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/engine.hpp"

Engine::OpenGLRenderer::OpenGLRenderer() noexcept: OpenGLRenderer(new OpenGLShader("Resources/Shaders/Basic3D.vsh", "Resources/Shaders/Basic3D.fsh"))
{
	ImGui_ImplGlfwGL3_Init(Engine::GetWindow().GetGLFWWindow());
}

Engine::OpenGLRenderer::OpenGLRenderer(OpenGLShader* shader) noexcept : shader(shader)
{
	projParam = shader->GetParameter("u_projection");
	modelParam = shader->GetParameter("u_model");
	viewParam = shader->GetParameter("u_view");
	textureParam = shader->GetParameter("u_texture");
	mainTextureColor = shader->GetParameter("u_mainTextColor");

	positionAttrib = shader->GetAttribute("a_position");
	normalAttrib = shader->GetAttribute("a_normal");
	textureAttrib = shader->GetAttribute("a_texture");

	ImGui_ImplGlfwGL3_Init(Engine::GetWindow().GetGLFWWindow());
}

Engine::OpenGLRenderer::~OpenGLRenderer() noexcept
{
	//ImGui_ImplGlfwGL3_Shutdown();
}

void Engine::OpenGLRenderer::RendererBegin(const glm::mat4x4 & view, const glm::mat4x4 & projection)
{
	ImGui_ImplGlfwGL3_NewFrame();
	//glViewport(0, 0, Engine::Engine::GetWindow().GetWidth(), Engine::Engine::GetWindow().GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->Activate();

	viewParam->SetValue(view);
	projParam->SetValue(projection);
}

void Engine::OpenGLRenderer::Render(const glm::mat4x4& modelMatrix,  Model* model, const glm::vec4& mainColor)
{
	modelParam->SetValue(modelMatrix);
	mainTextureColor->SetValue(mainColor);

	eastl::vector<Mesh*>& meshes = model->GetModelMeshes();
	for (size_t i = 0, size = meshes.size(); i < size; ++i)
	{
		if (glIsBuffer(GLuint(meshes[i]->GetVBO())) != GL_TRUE)
		{
			_ASSERT("The vertex buffer is not a valid buffer (VBO).");
		}
		if (glIsBuffer(GLuint(meshes[i]->GetVAO())) != GL_TRUE)
		{
			_ASSERT("The index buffer is not a valid buffer (VAO)");
		}

		// Bind the buffers to the global state
		glBindBuffer(GL_ARRAY_BUFFER, GLuint(meshes[i]->GetVBO()));
		glGetError();

		// Default to VBO values, the pointer addresses are interpreted as byte-offsets.
		const void* firstPosition = reinterpret_cast<const void*>(offsetof(Vertex, position));
		const void* firstNormal = reinterpret_cast<const void*>(offsetof(Vertex, normal));
		const void* firstTexture = reinterpret_cast<const void*>(offsetof(Vertex, texCoords));

		GLsizei stride = sizeof(Vertex);
		positionAttrib->SetAttributePointer(3, GL_FLOAT, GL_FALSE, stride, firstPosition);
		normalAttrib->SetAttributePointer(3, GL_FLOAT, GL_FALSE, stride, firstNormal);
		textureAttrib->SetAttributePointer(2, GL_FLOAT, GL_FALSE, stride, firstTexture);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLuint(meshes[i]->GetEBO()));
		glGetError();

		//USE TEXTURE
		if(meshes[i]->GetTextures().size() > 0)
			textureParam->SetValue(meshes[i]->GetTextures()[0]);
		glGetError();

		glDrawElements(GL_TRIANGLES, GLsizei(meshes[i]->indices.size()), GL_UNSIGNED_SHORT, reinterpret_cast<const void*>(0));
		glGetError();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glGetError();

		positionAttrib->DisableAttributePointer();
		normalAttrib->DisableAttributePointer();
		textureAttrib->DisableAttributePointer();
	}
}

void Engine::OpenGLRenderer::RendererEnd()
{
	ImGui::Render();
	shader->Deactivate();
	Engine::Engine::GetWindow().SwapBuffers();
}
