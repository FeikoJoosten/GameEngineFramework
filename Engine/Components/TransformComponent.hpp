#pragma once

#include "Engine/Components/Component.hpp"
#include <ThirdParty/glm/glm/glm.hpp>

namespace Engine
{
	class ENGINE_API TransformComponent : public Component
	{
	public:
		explicit TransformComponent() noexcept;
		explicit TransformComponent(bool isStatic) noexcept;
		explicit TransformComponent(glm::vec3 position) noexcept;
		explicit TransformComponent(glm::vec3 position, bool isStatic) noexcept;
		explicit TransformComponent(glm::vec3 position, glm::vec3 rotation) noexcept;
		explicit TransformComponent(glm::vec3 position, glm::vec3 rotation, bool isStatic) noexcept;
		explicit TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) noexcept;
		explicit TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool isStatic) noexcept;

		void SetPosition(glm::vec3 position) noexcept;
		void SetPosition(float x, float y, float z) noexcept;
		glm::vec3 GetPosition() noexcept;

		void SetRotation(glm::vec3 rotation) noexcept;
		void SetRotation(float x, float y, float z) noexcept;
		glm::vec3 GetRotation() noexcept;

		void SetScale(glm::vec3 scale) noexcept;
		void SetScale(float x, float y, float z) noexcept;
		glm::vec3 GetScale() noexcept;

		void SetModelMatrix(glm::mat4x4 modelMatrix) noexcept;
		void SetModelMatrix(float modelMatrix[16]) noexcept;
		void SetModelMatrix(glm::vec4 modelMatrix[4]) noexcept;
		glm::mat4x4 GetModelMatrix() noexcept;

		void SetIsStatic(bool isStatic) noexcept;
		bool GetIsStatic() noexcept;

		void Translate(glm::vec3 positionToAdd) noexcept;
		void Translate(float x) noexcept;
		void Translate(float x, float y) noexcept;
		void Translate(float x, float y, float z) noexcept;

		void AddRotate(glm::vec3 rotationToAdd) noexcept;
		void AddRotate(float x) noexcept;
		void AddRotate(float x, float y) noexcept;
		void AddRotate(float x, float y, float z) noexcept;

		void AddScale(glm::vec3 scaleToAdd) noexcept;
		void AddScale(float x) noexcept;
		void AddScale(float x, float y) noexcept;
		void AddScale(float x, float y, float z) noexcept;

		glm::vec3 GetRight() noexcept;
		glm::vec3 GetUp() noexcept;
		glm::vec3 GetForward() noexcept;

		void LookAt(TransformComponent target) noexcept;
		void LookAt(glm::vec3 targetPosition) noexcept;
		void RotateAround(float angle, glm::vec3 axis) noexcept;

		void Update() override;
		void UpdateModelMatrix() noexcept;

		bool operator!=(TransformComponent& other);
		bool operator==(TransformComponent& other);


	private:

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		bool isStatic;
		bool shouldRecalculateModelMatrix = false;

		glm::mat4x4 modelMatrix;
	};
} // namespace Engine