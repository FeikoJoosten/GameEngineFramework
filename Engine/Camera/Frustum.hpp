#pragma once
#include "Engine/api.hpp"
#include <ThirdParty/glm/glm/glm.hpp>

namespace Engine
{
	class Camera;

	/// <summary>
	/// This object is used to keep track of the frustum area of the view and projection matrix. NOTE: only the Camera class is allowed to create this object.
	/// </summary>
	class ENGINE_API Frustum
	{
	public:
		Frustum(Frustum const &other) = default;
		Frustum(Frustum &&other) noexcept = default;
		~Frustum() noexcept = default;

		Frustum &operator=(Frustum const &other) = default;
		Frustum &operator=(Frustum &&other) noexcept = default;

	private:
		friend class Camera;
		Frustum() = default;
		/// <summary>
		/// The constructor of a frustum. Allows you to create a frustum required for frustum culling.
		/// Only the Camera class is allowed to create a frustum.
		/// </summary>
		/// <param name="camera">The camera to create a frustum for.</param>
		/// <param name="right">The right direction of the frustum.</param>
		/// <param name="up">The up direction of the frustum.</param>
		explicit Frustum(Camera &camera, glm::vec3 right, glm::vec3 up);

		struct Face
		{
			glm::vec3 position0, position1, position2, normal, u, v;
			float d;

			Face() = default;
			Face(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
			~Face() = default;
		};

		Face frontFace, backFace, topFace, rightFace, bottomFace, leftFace;

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