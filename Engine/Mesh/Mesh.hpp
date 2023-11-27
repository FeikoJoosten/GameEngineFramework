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

		Mesh() = default;

	public:
		virtual ~Mesh() override = default;
		Mesh(const Mesh& other) = delete;
		Mesh(Mesh&& other) noexcept = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh& operator=(Mesh&& other) noexcept = delete;

		[[nodiscard]] const std::vector<glm::vec3>& GetVertices() const;

		[[nodiscard]] const std::vector<glm::vec3>& GetNormals() const;

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
		template <class Archive>
		void Serialize(Archive& archive);
	};

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
