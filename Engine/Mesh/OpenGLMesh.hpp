#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Mesh/Mesh.hpp"
#include <ThirdParty/glew-2.1.0/include/GL/glew.h>

namespace Engine
{
	/// <summary>
	/// This object is used to store data regarding a mesh. NOTE: only the resource manager is allowed to create a mesh.
	/// </summary>
	class ENGINE_API OpenGLMesh : public Mesh
	{
		friend class ResourceManager;

		OpenGLMesh() = delete;
		OpenGLMesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices);
		OpenGLMesh(OpenGLMesh const &other) = default;
		//OpenGLMesh(OpenGLMesh &&other) noexcept = default;
	public:
		~OpenGLMesh() noexcept = default;
	private:

		void SetUpMesh() override;
	};
} // namespace Engine
#endif