#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Mesh/OpenGLMesh.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"

#include <GL/glew.h>

namespace Engine
{
	void OpenGLMesh::SetUpMesh()
	{
		// Bind Vertices, texture coordinates and normals
		// Allocate one buffer
		glGenBuffers(1, &vbo);
		glCheckError();

		// Array buffer contains the attribute data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glCheckError();

		// Copy into VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glCheckError();
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer
		glCheckError();
		//--

		vbo = static_cast<uint64_t>(vbo);

		// Allocate one buffer
		glGenBuffers(1, &ebo);
		glCheckError();

		// Element array buffer contains the indices.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glCheckError();

		// Copy into VBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
		glCheckError();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1); // Unbind buffer
		glCheckError();

		ebo = uint64_t(ebo);

		//Could clear vertices & indices here
	}
}
#endif