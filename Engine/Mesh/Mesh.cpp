#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Engine/Engine.hpp"

namespace Engine
{
	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices)
	{
		//assign the relevant members
		this->vertices = vertices;
		this->indices = indices;
	}

	void Mesh::SetUpMesh()
	{
	}

	Mesh::~Mesh() noexcept
	{

	}

	uint64_t Mesh::GetVAO() const
	{
		return vao;
	}

	uint64_t Mesh::GetVBO() const
	{
		return vbo;
	}

	uint64_t Mesh::GetEBO() const
	{
		return ebo;
	}

	uint64_t Mesh::GetUBO() const
	{
		return ubo;
	}

	bool Mesh::operator==(Mesh& other)
	{
		if (name != other.name) return false;

		size_t thisVerticesSize = vertices.size();
		size_t otherVerticesSize = other.vertices.size();

		if (thisVerticesSize != otherVerticesSize) return false;

		size_t thisIndicesSize = indices.size();
		size_t otherIndicesSize = other.indices.size();

		if (thisIndicesSize != otherIndicesSize) return false;

		for (size_t i = 0; i < thisIndicesSize; ++i)
		{
			if (indices[i] != other.indices[i]) return false;
		}

		for (size_t i = 0; i < thisVerticesSize; ++i)
		{
			if (vertices[i].normal != other.vertices[i].normal) return false;
			if (vertices[i].position != other.vertices[i].position) return false;
			if (vertices[i].texCoords != other.vertices[i].texCoords) return false;
		}

		return true;
	}

	bool Mesh::operator!=(Mesh& other)
	{
		if (name != other.name) return true;

		size_t thisVerticesSize = vertices.size();
		size_t otherVerticesSize = other.vertices.size();

		if (thisVerticesSize != otherVerticesSize) return true;

		size_t thisIndicesSize = indices.size();
		size_t otherIndicesSize = other.indices.size();

		if (thisIndicesSize != otherIndicesSize) return true;

		for (size_t i = 0; i < thisIndicesSize; ++i)
		{
			if (indices[i] != other.indices[i]) return true;
		}

		for (size_t i = 0; i < thisVerticesSize; ++i)
		{
			if (vertices[i].normal != other.vertices[i].normal) return true;
			if (vertices[i].position != other.vertices[i].position) return true;
			if (vertices[i].texCoords != other.vertices[i].texCoords) return true;
		}

		return false;
	}
} // namespace Engine
