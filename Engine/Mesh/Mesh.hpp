#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Utility/Defines.hpp"

#include <cereal/types/base_class.hpp>
#include <cereal/types/vector.hpp>

namespace Engine
{
	class ENGINE_API Mesh : public Asset
	{
		friend cereal::access;
		friend class MeshAssetImporter;

	protected:
		std::vector<glm::vec4> colors {};
		std::vector<unsigned> indices {};
		std::vector<glm::vec3> normals {};
		std::vector<glm::vec3> tangents {};
		std::vector<glm::vec2> uvs {};
		std::vector<glm::vec2> uv2 {};
		std::vector<glm::vec2> uv3 {};
		std::vector<glm::vec2> uv4 {};
		std::vector<glm::vec2> uv5 {};
		std::vector<glm::vec2> uv6 {};
		std::vector<glm::vec2> uv7 {};
		std::vector<glm::vec2> uv8 {};
		std::vector<glm::vec3> vertices {};

		unsigned int ebo {};
		unsigned int ubo {};
		unsigned int vao {};
		unsigned int vbo {};

		Mesh() = default;

	public:
		virtual ~Mesh() override = default;
		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) noexcept = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) noexcept = delete;

		/// <summary>
		/// This method allows you to get the VAO of this mesh.
		/// </summary>
		/// <returns>Returns the VAO as an uint64_t.</returns>
		uint64_t GetVao() const;

		/// <summary>
		/// This method allows you to get the VAO of this mesh.
		/// </summary>
		/// <returns>Returns the VAO as the defined type.</returns>
		template<typename T>
		T GetVao();

		/// <summary>
		/// This method allows you to get the VBO of this mesh.
		/// </summary>
		/// <returns>Returns the VBO as an uint64_t.</returns>
		uint64_t GetVbo() const;

		/// <summary>
		/// This method allows you to get the VBO of this mesh.
		/// </summary>
		/// <returns>Returns the VBO as the defined type.</returns>
		template<typename T>
		T GetVbo();

		/// <summary>
		/// This method allows you to get the EBO of this mesh.
		/// </summary>
		/// <returns>Returns the EBO as an uint64_t.</returns>
		uint64_t GetEbo() const;

		/// <summary>
		/// This method allows you to get the EBO of this mesh.
		/// </summary>
		/// <returns>Returns the EBO as the defined type.</returns>
		template <typename T>
		T GetEbo();

		/// <summary>
		/// This method allows you to get the UBO of this mesh.
		/// </summary>
		/// <returns>Returns the UBO as an uint64_t.</returns>
		uint64_t GetUbo() const;

		/// <summary>
		/// This method allows you to get the UBO of this mesh.
		/// </summary>
		/// <returns>Returns the UBO as the defined type.</returns>
		template <typename T>
		T GetUbo();

		[[nodiscard]] const std::vector<glm::vec3>& GetVertices() const;

		[[nodiscard]] const std::vector<unsigned>& GetIndices() const;

		[[nodiscard]] const std::vector<glm::vec2>& GetUv() const;

		[[nodiscard]] const std::vector<glm::vec2>& GetUv2() const;

		[[nodiscard]] const std::vector<glm::vec2>& GetUv3() const;

		[[nodiscard]] const std::vector<glm::vec2>& GetUv4() const;

		[[nodiscard]] const std::vector<glm::vec2>& GetUv5() const;

		[[nodiscard]] const std::vector<glm::vec2>& GetUv6() const;

		[[nodiscard]] const std::vector<glm::vec2>& GetUv7() const;

		[[nodiscard]] const std::vector<glm::vec2>& GetUv8() const;

	private:
		virtual void SetUpMesh();

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <typename T>
	T Mesh::GetVao()
	{
		return T(vao);
	}

	template <typename T>
	T Mesh::GetVbo()
	{
		return T(vbo);
	}

	template <typename T>
	T Mesh::GetEbo()
	{
		return T(ebo);
	}

	template <typename T>
	T Mesh::GetUbo()
	{
		return T(ubo);
	}

	template <class Archive>
	void Mesh::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(cereal::base_class<Asset>(this)),
			CEREAL_NVP(colors),
			CEREAL_NVP(indices),
			CEREAL_NVP(normals),
			CEREAL_NVP(tangents),
			CEREAL_NVP(uvs),
			CEREAL_NVP(uv2),
			CEREAL_NVP(uv3),
			CEREAL_NVP(uv4),
			CEREAL_NVP(uv5),
			CEREAL_NVP(uv6),
			CEREAL_NVP(uv7),
			CEREAL_NVP(uv8),
			CEREAL_NVP(vertices)
		);
	}

}
