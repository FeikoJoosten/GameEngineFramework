#ifndef MESH_HPP
#define MESH_HPP
#pragma once

#include "Engine/api.hpp"
#include "Engine/Texture/Texture.hpp"
#include "Engine/Utility/Vertex.hpp"

#include <assimp/Importer.hpp>
#include <ThirdParty/glm/glm/glm.hpp>
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	class ENGINE_API Mesh
	{
	public:
		eastl::vector<Vertex> vertices;
		eastl::vector<unsigned> indices;

		Mesh() = delete;
		Mesh(eastl::vector<Vertex> vertices, eastl::vector<unsigned> indices, eastl::vector<Texture*> textures);
		//Mesh(Mesh const &other) = default;
		//Mesh(Mesh &&other) noexcept = default;
		virtual ~Mesh() noexcept;

		uint64_t GetVAO();
		template<typename T>
		T GetVAO();

		uint64_t GetVBO();
		template<typename T>
		T GetVBO();

		uint64_t GetEBO();
		template <typename T>
		T GetEBO();

		uint64_t GetUBO();
		template <typename T>
		T GetUBO();

		eastl::vector<Texture*> GetTextures();
		virtual void AddTexture(eastl::string textureName);

	private:
		//render data
		//TODO use this function to set up additional variables that you might need for the mesh
		virtual void SetUpMesh();
	protected:
		uint64_t vao, vbo, ebo, ubo;
		eastl::vector<Texture*> textures;

	};

	template <typename T>
	T Mesh::GetVAO()
	{
		return T(vao);
	}

	template <typename T>
	T Mesh::GetVBO()
	{
		return T(vbo);
	}

	template <typename T>
	T Mesh::GetEBO()
	{
		return T(ebo);
	}

	template <typename T>
	T Mesh::GetUBO()
	{
		return T(ubo);
	}

} // namespace Engine
#endif
