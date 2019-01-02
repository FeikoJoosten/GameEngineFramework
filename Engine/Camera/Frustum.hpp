#pragma once
#include "Engine/api.hpp"
#include <ThirdParty/glm/glm/glm.hpp>

namespace Engine
{
	class Camera;

	class ENGINE_API Frustum
	{
	public:
		Frustum() = default;
		explicit Frustum(Camera &camera, glm::vec3 right, glm::vec3 up);
		Frustum(Frustum const &other) = default;
		Frustum(Frustum &&other) noexcept = default;
		~Frustum() noexcept = default;

		Frustum &operator=(Frustum const &other) = default;
		Frustum &operator=(Frustum &&other) noexcept = default;

	private:
		struct Face
		{
			glm::vec3 position0, position1, position2, normal, u, v;
			float d;

			Face() = default;
			Face(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
			~Face() = default;
		};

		Face frontFace, backFace, topFace, rightFace, bottomFace, leftFace;
	};
} // namespace Engine