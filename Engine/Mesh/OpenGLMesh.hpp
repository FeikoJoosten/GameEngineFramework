#pragma once

#include "Engine/Mesh/Mesh.h"

namespace Engine
{
	class ENGINE_API OpenGLMesh : public Mesh
	{
	public:
		OpenGLMesh() = delete;
		OpenGLMesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices, eastl::vector<Texture*> textures);
		OpenGLMesh(OpenGLMesh const &other) = default;
		//OpenGLMesh(OpenGLMesh &&other) noexcept = default;
		~OpenGLMesh() noexcept = default;
		void AddTexture(eastl::string textureName) override;
	
		private:
		void SetUpMesh() override;
	};
} // namespace Engine