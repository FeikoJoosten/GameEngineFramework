#pragma once
#include "Engine/Api/Api.hpp"
#include <glm/glm.hpp>

namespace Engine {
	/// <summary>
	/// This object is used to keep track of the frustum area of the view and projection matrix.
	/// </summary>
	struct ENGINE_API Frustum {
		Frustum() = default;
		Frustum(const Frustum& other) = default;
		Frustum(Frustum&& other) noexcept = default;
		~Frustum() noexcept = default;

		Frustum& operator=(const Frustum& other) = default;
		Frustum& operator=(Frustum&& other) noexcept = default;

		/// <summary>
		/// The constructor of a frustum. Allows you to create a frustum required for frustum culling.
		/// </summary>
		/// <param name="fieldOfVision">The field of vision of the frustum</param>
		/// <param name="aspectRatio">The aspect ration of the frustum</param>
		/// <param name="position">The position of the frustum</param>
		/// <param name="rotation">The rotation of the frustum</param>
		/// <param name="clippingPlanes">The clipping planes of the frustum</param>
		/// <param name="right">The right direction of the frustum.</param>
		/// <param name="up">The up direction of the frustum.</param>
		explicit Frustum(float fieldOfVision, float aspectRatio, glm::vec3 position, glm::quat rotation, glm::vec2 clippingPlanes, glm::vec3 right, glm::vec3 up);

	private:
		struct ENGINE_API Face {
			glm::vec3 position0;
			glm::vec3 position1;
			glm::vec3 position2;
			glm::vec3 normal;
			glm::vec3 u;
			glm::vec3 v;

			Face() = default;
			Face(const Face& other) = default;
			Face(Face&& other) noexcept = default;
			Face(glm::vec3 position0, glm::vec3 position1, glm::vec3 position2);
			~Face() noexcept = default;

			Face& operator=(const Face& other) = default;
			Face& operator=(Face&& other) noexcept = default;
		};

		Face frontFace;
		Face backFace;
		Face topFace;
		Face rightFace;
		Face bottomFace;
		Face leftFace;

	public:
		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the front face of the frustum</returns>
		Face GetFrontFace() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the back face of the frustum</returns>
		Face GetBackFace() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the top face of the frustum</returns>
		Face GetTopFace() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the right face of the frustum</returns>
		Face GetRightFace() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the bottom face of the frustum</returns>
		Face GetBottomFace() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns>Returns the left face of the frustum</returns>
		Face GetLeftFace() const;
	};
} // namespace Engine