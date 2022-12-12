#include "Engine/Components/TransformComponent.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Engine {
	TransformComponent::TransformComponent() {
		RecalculateModelMatrix();
	}

	void TransformComponent::SetPosition(const glm::vec3 newPosition) noexcept {
		if (isStatic) return;

		position = newPosition;
		RecalculateModelMatrixAndInvokedOnModified();
	}

	void TransformComponent::SetPosition(const float x, const float y, const float z) noexcept {
		SetPosition(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetPosition() const noexcept {
		return position;
	}

	void TransformComponent::SetRotation(const glm::quat newRotation) noexcept {
		if (isStatic) return;

		rotation = newRotation;
		RecalculateModelMatrixAndInvokedOnModified();
	}

	void TransformComponent::SetRotation(const float x, const float y, const float z) noexcept {
		SetRotation(glm::radians(glm::vec3(x, y, z)));
	}

	glm::quat TransformComponent::GetRotation() const noexcept {
		return rotation;
	}

	void TransformComponent::SetPositionAndRotation(const glm::vec3 newPosition, const glm::vec3 newRotation) {
		SetPositionAndRotation(newPosition, glm::quat(glm::radians(newRotation)));
	}

	void TransformComponent::SetPositionAndRotation(const glm::vec3 newPosition, const glm::quat newRotation) {
		if (isStatic) return;

		position = newPosition;
		rotation = newRotation;
		RecalculateModelMatrixAndInvokedOnModified();
	}

	void TransformComponent::SetScale(const glm::vec3 newScale) noexcept {
		if (isStatic) return;

		scale = newScale;
		RecalculateModelMatrixAndInvokedOnModified();
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
		DecomposeModelMatrix();
	}

	glm::mat4x4 TransformComponent::GetModelMatrix() const noexcept {
		return modelMatrix;
	}

	void TransformComponent::SetIsStatic(const bool newIsStatic) noexcept {
		isStatic = newIsStatic;

		OnModifiedEvent(std::static_pointer_cast<TransformComponent>(shared_from_this()));
	}

	bool TransformComponent::GetIsStatic() const noexcept {
		return isStatic;
	}

	void TransformComponent::Translate(const glm::vec3 positionToAdd) noexcept {
		if (isStatic) return;

		position += rotation * positionToAdd;
		RecalculateModelMatrixAndInvokedOnModified();
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

	void TransformComponent::Rotate(const glm::vec3 rotationToAdd) noexcept {
		Rotate(glm::quat(glm::radians(rotationToAdd)));
	}

	void TransformComponent::Rotate(const glm::quat rotationToAdd) noexcept {
		if (isStatic) return;

		rotation *= rotationToAdd;
		RecalculateModelMatrixAndInvokedOnModified();
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
		RecalculateModelMatrixAndInvokedOnModified();
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

		modelMatrix = glm::lookAt(position, targetPosition, GetUp());
		DecomposeModelMatrix();
	}

	void TransformComponent::DecomposeModelMatrix() noexcept {
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(modelMatrix, scale, rotation, position, skew, perspective);
		rotation = glm::conjugate(rotation);
		
		OnModifiedEvent(std::static_pointer_cast<TransformComponent>(shared_from_this()));
	}

	void TransformComponent::RecalculateModelMatrix() noexcept {
		constexpr glm::mat4 identityMatrix = glm::identity<glm::mat4>();
		const glm::mat4 translationMatrix = glm::translate(identityMatrix, position);
		const glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
		const glm::mat4 scaleMatrix = glm::scale(identityMatrix, scale);

		const glm::mat4 newModelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

		modelMatrix = newModelMatrix;
	}

	void TransformComponent::RecalculateModelMatrixAndInvokedOnModified() noexcept {
		RecalculateModelMatrix();
		OnModifiedEvent(std::static_pointer_cast<TransformComponent>(shared_from_this()));
	}
} //namespace Engine