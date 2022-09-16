#include "Engine/Components/TransformComponent.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Engine {
	TransformComponent::TransformComponent() noexcept : TransformComponent(
		glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), false) {}

	TransformComponent::TransformComponent(const bool isStatic) noexcept : TransformComponent(
		glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), isStatic) {}

	TransformComponent::TransformComponent(const glm::vec3 position) noexcept : TransformComponent(
		position, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), false) {}

	TransformComponent::TransformComponent(const glm::vec3 position, const bool isStatic) noexcept : TransformComponent(
		position, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), isStatic) {}

	TransformComponent::TransformComponent(const glm::vec3 position, const glm::quat rotation) noexcept : TransformComponent(
		position, rotation, glm::vec3(1, 1, 1), false) {}

	TransformComponent::TransformComponent(const glm::vec3 position, const glm::quat rotation, const bool isStatic) noexcept :
		TransformComponent(position, rotation, glm::vec3(1, 1, 1), isStatic) {}

	TransformComponent::TransformComponent(const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale) noexcept :
		TransformComponent(position, rotation, scale, false) {}

	TransformComponent::TransformComponent(const glm::vec3 position, const glm::quat rotation, const glm::vec3 scale,
		const bool isStatic) noexcept : position(position), rotation(rotation),
		scale(scale), isStatic(isStatic) {
		UpdateModelMatrix();
	}

	void TransformComponent::SetPosition(const glm::vec3 newPosition) noexcept {
		if (isStatic) return;

		position = newPosition;
		UpdateModelMatrix();
	}

	void TransformComponent::SetPosition(const float x, const float y, const float z) noexcept {
		SetPosition(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetPosition() const noexcept {
		return position;
	}

	void TransformComponent::SetRotation(const glm::vec3 newRotation) noexcept {
		if (isStatic) return;

		rotation = newRotation;
		UpdateModelMatrix();
	}

	void TransformComponent::SetRotation(const float x, const float y, const float z) noexcept {
		SetRotation(glm::vec3(x, y, z));
	}

	glm::quat TransformComponent::GetRotation() const noexcept {
		return rotation;
	}

	void TransformComponent::SetPositionAndRotation(glm::vec3 newPosition, glm::vec3 newRotation) {
		if (isStatic) return;

		position = newPosition;
		rotation = newRotation;
		UpdateModelMatrix();
	}

	void TransformComponent::SetScale(const glm::vec3 newScale) noexcept {
		if (isStatic) return;

		scale = newScale;
		UpdateModelMatrix();
	}

	void TransformComponent::SetScale(const float x, const float y, const float z) noexcept {
		SetScale(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetScale() const noexcept {
		return scale;
	}

	void TransformComponent::SetModelMatrix(const glm::mat4x4 newModelMatrix) noexcept {
		if (isStatic) return;

		modelMatrix = newModelMatrix;
		OnModifiedEvent();
	}

	void TransformComponent::SetModelMatrix(float newModelMatrix[16]) noexcept {
		SetModelMatrix(glm::mat4x4(newModelMatrix[0], newModelMatrix[1], newModelMatrix[2], newModelMatrix[3],
			newModelMatrix[4], newModelMatrix[5], newModelMatrix[6], newModelMatrix[7],
			newModelMatrix[8], newModelMatrix[9], newModelMatrix[10], newModelMatrix[11],
			newModelMatrix[12], newModelMatrix[13], newModelMatrix[14], newModelMatrix[15]));
	}

	void TransformComponent::SetModelMatrix(glm::vec4 newModelMatrix[4]) noexcept {
		SetModelMatrix(glm::mat4x4(newModelMatrix[0].x, newModelMatrix[0].y, newModelMatrix[0].z, newModelMatrix[0].w,
			newModelMatrix[1].x, newModelMatrix[1].y, newModelMatrix[1].z, newModelMatrix[1].w,
			newModelMatrix[2].x, newModelMatrix[2].y, newModelMatrix[2].z, newModelMatrix[2].w,
			newModelMatrix[3].x, newModelMatrix[3].y, newModelMatrix[3].z, newModelMatrix[3].w));
	}

	glm::mat4x4 TransformComponent::GetModelMatrix() const noexcept {
		return modelMatrix;
	}

	void TransformComponent::SetIsStatic(const bool newIsStatic) noexcept {
		isStatic = newIsStatic;
	}

	bool TransformComponent::GetIsStatic() const noexcept {
		return isStatic;
	}

	void TransformComponent::Translate(const glm::vec3 positionToAdd) noexcept {
		if (isStatic) return;

		position += rotation * positionToAdd;
		UpdateModelMatrix();
	}

	void TransformComponent::Translate(const float x) noexcept {
		Translate(glm::vec3(x, 0, 0));
	}

	void TransformComponent::Translate(const float x, const float y) noexcept {
		Translate(glm::vec3(x, y, 0));
	}

	void TransformComponent::Translate(const float x, const float y, const float z) noexcept {
		Translate(glm::vec3(x, y, z));
	}

	void TransformComponent::Rotate(const glm::quat rotationToAdd) noexcept {
		if (isStatic) return;

		rotation *= rotationToAdd;
		UpdateModelMatrix();
	}

	void TransformComponent::AddRotation(const float x) noexcept {
		Rotate(glm::vec3(x, 0, 0));
	}

	void TransformComponent::AddRotation(const float x, const float y) noexcept {
		Rotate(glm::vec3(x, y, 0));
	}

	void TransformComponent::AddRotation(const float x, const float y, const float z) noexcept {
		Rotate(glm::vec3(x, y, z));
	}

	void TransformComponent::AddScale(const glm::vec3 scaleToAdd) noexcept {
		if (isStatic) return;

		scale += scaleToAdd;
		UpdateModelMatrix();
	}

	void TransformComponent::AddScale(const float x) noexcept {
		AddScale(glm::vec3(x, 0, 0));
	}

	void TransformComponent::AddScale(const float x, const float y) noexcept {
		AddScale(glm::vec3(x, y, 0));
	}

	void TransformComponent::AddScale(const float x, const float y, const float z) noexcept {
		AddScale(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetRight() const noexcept {
		return rotation * glm::vec3(1.f, 0.f, 0.f);
	}

	glm::vec3 TransformComponent::GetUp() const noexcept {
		return rotation * glm::vec3(0.f, 1.f, 0.f);
	}

	glm::vec3 TransformComponent::GetForward() const noexcept {
		return rotation * glm::vec3(0.f, 0.f, 1.f);
	}

	void TransformComponent::LookAt(const TransformComponent& target) noexcept {
		LookAt(target.position);
	}

	void TransformComponent::LookAt(const glm::vec3 targetPosition) noexcept {
		if (isStatic) return;

		modelMatrix *= glm::lookAt(position, targetPosition, GetUp());
		OnModifiedEvent();
	}

	void TransformComponent::RotateAround(const float angle, const glm::vec3 axis) noexcept {
		if (isStatic) return;

		const glm::mat4x4 rotationMatrix;
		glm::rotate(rotationMatrix, angle, axis);
		modelMatrix *= rotationMatrix;
		OnModifiedEvent();
	}

	void TransformComponent::UpdateModelMatrix() noexcept {
		const glm::mat4x4 currentModelMatrix = modelMatrix;
		const glm::mat4x4 rotationMatrix = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
		glm::mat4x4 rotationScaleMatrix = rotationMatrix * glm::scale(rotationMatrix, scale);
		rotationScaleMatrix[3] = glm::vec4(position, 1);

		if (currentModelMatrix == rotationScaleMatrix) return;

		modelMatrix = rotationScaleMatrix;
		OnModifiedEvent();
	}

	bool TransformComponent::operator!=(const TransformComponent& other) const {
		if (position != other.position) return true;
		if (rotation != other.rotation) return true;
		if (scale != other.scale) return true;
		return false;
	}

	bool TransformComponent::operator==(const TransformComponent& other) const {
		if (position != other.position) return false;
		if (rotation != other.rotation) return false;
		if (scale != other.scale) return false;
		return true;
	}
} //namespace Engine