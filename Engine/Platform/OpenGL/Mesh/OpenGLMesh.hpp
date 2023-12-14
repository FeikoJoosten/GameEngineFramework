#pragma once

#include "Engine/Utility/Defines.hpp"

#ifdef USING_OPENGL
#include "Engine/Mesh/Mesh.hpp"

#include <GL/glew.h>

namespace Engine
{
	class ENGINE_LOCAL OpenGLMesh
	{
		friend class OpenGLRenderer;

		std::shared_ptr<Mesh> mesh;
		GLuint ebo {};
		GLuint vbo {};

		explicit OpenGLMesh(const std::shared_ptr<Mesh>& mesh);

	public:
		OpenGLMesh() = delete;
		virtual ~OpenGLMesh() noexcept = default;
		OpenGLMesh(const OpenGLMesh&other) = delete;
		OpenGLMesh(OpenGLMesh &&other) noexcept = delete;

		OpenGLMesh& operator=(const OpenGLMesh& other) = delete;
		OpenGLMesh& operator=(OpenGLMesh&& other) noexcept = delete;
	};
}
#endif
