#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Mesh/OpenGLMesh.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"

#include <iostream>
#include <GL/glew.h>

namespace Engine
{
	OpenGLMesh::OpenGLMesh(std::vector<Vertex> vertices, std::vector<unsigned> indices) : Mesh(vertices, indices)
	{
		OpenGLMesh::SetUpMesh();
	}

	void OpenGLMesh::SetUpMesh()
	{
		// Bind Vertices, texture coordinates and normals
		// Allocate one buffer
		unsigned int uVBO = unsigned int(vbo);
		glGenBuffers(1, &uVBO);
		glCheckError();

		// Array buffer contains the attribute data
		glBindBuffer(GL_ARRAY_BUFFER, uVBO);
		glCheckError();

		// Copy into VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glCheckError();
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer
		glCheckError();
		//--

		vbo = uint64_t(uVBO);

		unsigned int uEBO = unsigned int(ebo);
		// Allocate one buffer
		glGenBuffers(1, &uEBO);
		glCheckError();

		// Element array buffer contains the indices.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uEBO);
		glCheckError();

		// Copy into VBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
		glCheckError();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1); // Unbind buffer
		glCheckError();

		ebo = uint64_t(uEBO);

		//Could clear vertices & indices here
	}
} // namespace Engine
#endif