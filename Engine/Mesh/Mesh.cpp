#include "Engine/Mesh/Mesh.h"
#include "Engine/Resources/ModelLoader.hpp"

namespace Engine
{
	Mesh::Mesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices, eastl::vector<Texture*> textures)
	{
		//assign the relevant members
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		//set up the mesh - vbo, vao, ebo
		Mesh::SetUpMesh();
	}

	void Mesh::SetUpMesh()
	{
	}

	Mesh::~Mesh() noexcept
	{
	}

	uint64_t Mesh::GetVAO()
	{
		return vao;
	}
	
	uint64_t Mesh::GetVBO()
	{
		return vbo;
	}

	uint64_t Mesh::GetEBO()
	{
		return ebo;
	}

	uint64_t Mesh::GetUBO()
	{
		return ubo;
	}

	eastl::vector<Texture*> Mesh::GetTextures()
	{
		return textures;
	}

	void Mesh::AddTexture(eastl::string textureName)
	{
		textures.push_back(ModelLoader::CreateTexture(textureName));
	}
} // namespace Engine
