#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Texture/Texture.hpp"
#include "Engine/Utility/Vertex.hpp"
#include "Engine/Utility/Defines.hpp"

#include <vector>

namespace Engine
{
	/// <summary>
	/// This object is used to store data regarding a mesh. NOTE: only the resource manager is allowed to create a mesh.
	/// </summary>
	class ENGINE_API Mesh : public Asset
	{
		friend cereal::access;

	public:
		/// <summary>
		/// The vertices of this mesh.
		/// </summary>
		std::vector<Vertex> vertices;
		/// <summary>
		/// The indices of this mesh.
		/// </summary>
		std::vector<unsigned> indices;

		/// <summary>
		/// This method allows you to get the VAO of this mesh.
		/// </summary>
		/// <returns>Returns the VAO as an uint64_t.</returns>
		uint64_t GetVAO() const;
		template<typename T>
		/// <summary>
		/// This method allows you to get the VAO of this mesh.
		/// </summary>
		/// <returns>Returns the VAO as the defined type.</returns>
		T GetVAO();

		/// <summary>
		/// This method allows you to get the VBO of this mesh.
		/// </summary>
		/// <returns>Returns the VBO as an uint64_t.</returns>
		uint64_t GetVBO() const;
		template<typename T>
		/// <summary>
		/// This method allows you to get the VBO of this mesh.
		/// </summary>
		/// <returns>Returns the VBO as the defined type.</returns>
		T GetVBO();

		/// <summary>
		/// This method allows you to get the EBO of this mesh.
		/// </summary>
		/// <returns>Returns the EBO as an uint64_t.</returns>
		uint64_t GetEBO() const;
		template <typename T>
		/// <summary>
		/// This method allows you to get the EBO of this mesh.
		/// </summary>
		/// <returns>Returns the EBO as the defined type.</returns>
		T GetEBO();

		/// <summary>
		/// This method allows you to get the UBO of this mesh.
		/// </summary>
		/// <returns>Returns the UBO as an uint64_t.</returns>
		uint64_t GetUBO() const;
		template <typename T>
		/// <summary>
		/// This method allows you to get the UBO of this mesh.
		/// </summary>
		/// <returns>Returns the UBO as the defined type.</returns>
		T GetUBO();

		/// <summary>
		/// This method allows you to compare a mesh with another mesh.
		/// </summary>
		/// <param name="other">The mesh you want to compare against.</param>
		/// <returns>Returns true if both meshes are equal.</returns>
		bool operator==(Mesh& other);
		/// <summary>
		/// This method allows you to compare a mesh with another mesh.
		/// </summary>
		/// <param name="other">The mesh you want to compare against.</param>
		/// <returns>Returns true if the meshes are not equal.</returns>
		bool operator!=(Mesh& other);

	private:

		friend class ResourceManager;
#ifdef USING_OPENGL
		friend class OpenGLMesh;
#endif
#ifdef USING_VULKAN
		friend class VulkanMesh;
#endif

		Mesh() = delete;
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned> indices);
		virtual ~Mesh() noexcept;

		//render data
		//TODO use this function to set up additional variables that you might need for the mesh
		virtual void SetUpMesh();
	protected:
		uint64_t vao, vbo, ebo, ubo;

	private:

		template <class Archive>
		void Serialize(Archive& archive);
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

	template <class Archive>
	void Mesh::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(vertices),
			CEREAL_NVP(indices),
			CEREAL_NVP(vao),
			CEREAL_NVP(vbo),
			CEREAL_NVP(ebo),
			CEREAL_NVP(ubo)
		);
	}

} // namespace Engine