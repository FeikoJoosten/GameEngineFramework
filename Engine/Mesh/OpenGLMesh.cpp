#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Mesh/OpenGLMesh.hpp"
#include "Engine/Renderer/OpenGLUtility.hpp"

namespace Engine
{
	struct VertexData {
		glm::vec3 vertices;
		glm::vec3 normals;
		glm::vec2 uv;
	};

	OpenGLMesh::OpenGLMesh(const std::shared_ptr<Mesh>& mesh)
	{
		this->mesh = mesh;

		// Bind Vertices, texture coordinates and normals
		// Allocate one buffer
		glGenBuffers(1, &vbo);
		glCheckError();

		// Array buffer contains the attribute data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glCheckError();

		// If the shader drawing this object no longer requires uvs/normals/etc this will fail to render
		std::vector<VertexData> data {};
		const std::vector<glm::vec3> vertices = mesh->GetVertices();
		const std::vector<glm::vec3> normals = mesh->GetNormals();
		const std::vector<glm::vec2> uvs = mesh->GetUv();

		for(size_t i = 0; i < vertices.size(); ++i)
			data.push_back({ vertices[i], normals[i], uvs[i] });

		// Copy into VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * data.size(), &data[0], GL_STATIC_DRAW);
		glCheckError();
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer
		glCheckError();
		//--

		// Allocate one buffer
		glGenBuffers(1, &ebo);
		glCheckError();

		// Element array buffer contains the indices.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glCheckError();

		// Copy into VBO
		const std::vector<unsigned>& indices = mesh->GetIndices();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
		glCheckError();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 1); // Unbind buffer
		glCheckError();
	}
}
#endif