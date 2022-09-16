#pragma once

#include "Engine/Utility/Defines.hpp"
#ifdef USING_OPENGL
#include "Engine/Mesh/Mesh.hpp"

namespace Engine
{
	/// <summary>
	/// This object is used to store data regarding a mesh. NOTE: only the resource manager is allowed to create a mesh.
	/// </summary>
	class ENGINE_API OpenGLMesh : public Mesh
	{
		friend class ResourceManager;

		OpenGLMesh(std::vector<Vertex> vertices, std::vector<unsigned> indices);
	public:
		OpenGLMesh() = delete;
		virtual ~OpenGLMesh() noexcept override = default;
		OpenGLMesh(const OpenGLMesh&other) = delete;
		OpenGLMesh(OpenGLMesh &&other) noexcept = delete;

		OpenGLMesh& operator=(const OpenGLMesh& other) = delete;
		OpenGLMesh& operator=(OpenGLMesh&& other) noexcept = delete;
	private:

		void SetUpMesh() override;
	};
} // namespace Engine
#endif