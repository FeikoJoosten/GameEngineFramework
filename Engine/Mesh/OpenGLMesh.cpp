#include "Engine/Mesh/OpenGLMesh.hpp"
#include <ThirdParty/glew-2.1.0/include/GL/glew.h>

namespace Engine
{
	OpenGLMesh::OpenGLMesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices,
		eastl::vector<Texture*> textures) : Mesh(vertices, indices, textures)
	{
		OpenGLMesh::SetUpMesh();
	}

	void OpenGLMesh::AddTexture(eastl::string textureName)
	{
		Mesh::AddTexture(textureName);
	}

	void OpenGLMesh::SetUpMesh()
	{
		// Bind Vertices, texture coordinates and normals
		// Allocate one buffer
		unsigned int uVBO = unsigned int(vbo);
		glGenBuffers(1, &uVBO);
		glGetError();

		// Array buffer contains the attribute data
		glBindBuffer(GL_ARRAY_BUFFER, uVBO);
		glGetError();

		// Copy into VBO
		glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glGetError();
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer
		glGetError();
		//--

		vbo = uint64_t(uVBO);

		unsigned int uEBO = unsigned int(ebo);
		// Allocate one buffer
		glGenBuffers(1, &uEBO);
		glGetError();

		// Element array buffer contains the indices.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uEBO);
		glGetError();

		// Copy into VBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indices.size(), &indices[0], GL_STATIC_DRAW);
		glGetError();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1); // Unbind buffer
		glGetError();

		ebo = uint64_t(uEBO);

		//Could clear vertices & indices here
	}
} // namespace Engine