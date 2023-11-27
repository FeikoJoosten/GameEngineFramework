#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Logging.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

CEREAL_REGISTER_TYPE(Engine::TransformComponent)

namespace Engine {
	TransformComponent::TransformComponent() {
		RecalculateLocalMatrix();
	}

	void TransformComponent::SetParent(const std::shared_ptr<TransformComponent>& newParent, const bool keepWorldPosition) {
		glm::vec3 oldPosition;
		glm::quat oldRotation;
		glm::vec3 oldScale;

		if (!isStatic) {
			if (keepWorldPosition) {
				oldPosition = GetPosition();
				oldRotation = GetRotation();
				oldScale = GetScale();
			} else {
				oldPosition = GetLocalPosition();
				oldRotation = GetLocalRotation();
				oldScale = GetLocalScale();
			}
		}

		parent = newParent;
		
		if (!isStatic) {
			constexpr glm::mat4 identityMatrix = glm::identity<glm::mat4>();
			const glm::mat4 translationMatrix = glm::translate(identityMatrix, oldPosition);
			const glm::mat4 rotationMatrix = glm::mat4_cast(oldRotation);
			const glm::mat4 scaleMatrix = glm::scale(identityMatrix, oldScale);

			const glm::mat4 newMatrix = translationMatrix * rotationMatrix * scaleMatrix;

			if (keepWorldPosition) SetMatrix(newMatrix);
			else SetLocalMatrix(newMatrix);
		}
	}

	std::shared_ptr<TransformComponent> TransformComponent::GetParent() {
		return parent;
	}

	void TransformComponent::SetPosition(const glm::vec3 newPosition) noexcept {
		if (isStatic) return;

		localPosition = parent ? GetPosition() - newPosition : newPosition;
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::SetPosition(const float x, const float y, const float z) noexcept {
		SetPosition({ x, y, z });
	}

	glm::vec3 TransformComponent::GetPosition() const noexcept {
		if (parent) return parent->GetPosition() + localPosition;

		return localPosition;
	}

	void TransformComponent::SetLocalPosition(const glm::vec3 newPosition) noexcept {
		if (isStatic) return;

		localPosition = newPosition;

		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::SetLocalPosition(const float x, const float y, const float z) noexcept {
		SetLocalPosition({ x, y, z });
	}

	glm::vec3 TransformComponent::GetLocalPosition() const noexcept {
		return localPosition;
	}

	void TransformComponent::SetRotation(const glm::quat newRotation) noexcept {
		if (isStatic) return;

		localRotation = parent ? glm::conjugate(parent->GetRotation()) * newRotation : newRotation;
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::SetRotation(const float x, const float y, const float z) noexcept {
		SetRotation(glm::radians(glm::vec3(x, y, z)));
	}

	glm::quat TransformComponent::GetRotation() const noexcept {
		if (parent) return parent->GetRotation() * localRotation;

		return localRotation;
	}

	void TransformComponent::SetLocalRotation(const glm::quat newRotation) noexcept {
		if (isStatic) return;

		localRotation = newRotation;

		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::SetLocalRotation(const float x, const float y, const float z) noexcept {
		SetLocalRotation(glm::radians(glm::vec3(x, y, z)));
	}

	glm::quat TransformComponent::GetLocalRotation() const noexcept {
		return localRotation;
	}

	void TransformComponent::SetPositionAndRotation(const glm::vec3 newPosition, const glm::vec3 newRotation) {
		SetPositionAndRotation(newPosition, glm::quat(glm::radians(newRotation)));
	}

	void TransformComponent::SetPositionAndRotation(const glm::vec3 newPosition, const glm::quat newRotation) {
		if (isStatic) return;

		localPosition = parent ? GetPosition() - newPosition : newPosition;
		localRotation = parent ? glm::conjugate(parent->GetRotation()) * newRotation : newRotation;
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::SetLocalPositionAndLocalRotation(const glm::vec3 newPosition, const glm::vec3 newRotation) {
		SetLocalPositionAndLocalRotation(newPosition, glm::quat(glm::radians(newRotation)));
	}

	void TransformComponent::SetLocalPositionAndLocalRotation(const glm::vec3 newPosition, const glm::quat newRotation) {
		if (isStatic) return;

		localPosition = newPosition;
		localRotation = newRotation;
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::SetScale(const glm::vec3 newScale) noexcept {
		if (isStatic) return;

		localScale = parent ? GetScale() * glm::vec3(1.f) / parent->GetScale() : newScale;
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::SetScale(const float x, const float y, const float z) noexcept {
		SetScale(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetScale() const noexcept {
		if (parent) return parent->GetScale() * localScale;

		return localScale;
	}

	void TransformComponent::SetLocalScale(const glm::vec3 newScale) noexcept {
		if (isStatic) return;

		localScale = newScale;
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::SetLocalScale(const float x, const float y, const float z) noexcept {
		SetLocalScale({ x, y, z });
	}

	glm::vec3 TransformComponent::GetLocalScale() const noexcept {
		return localScale;
	}

	void TransformComponent::SetMatrix(const glm::mat4x4 newMatrix) noexcept {
		if (isStatic) return;

		localMatrix = parent ? parent->GetMatrix() * glm::inverse(GetMatrix()) : newMatrix;
		
		DecomposeLocalMatrix();
	}

	glm::mat4x4 TransformComponent::GetMatrix() const noexcept {
		if (parent) return localMatrix * parent->GetMatrix();

		return localMatrix;
	}

	void TransformComponent::SetLocalMatrix(const glm::mat4x4 newMatrix) noexcept {
		if (isStatic) return;

		localMatrix = newMatrix;
		DecomposeLocalMatrix();
	}

	glm::mat4x4 TransformComponent::GetLocalMatrix() const noexcept {
		return localMatrix;
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

		localPosition += localRotation * positionToAdd;
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::Translate(const float x) noexcept {
		Translate({ x, 0, 0 });
	}

	void TransformComponent::Translate(const float x, const float y) noexcept {
		Translate({ x, y, 0 });
	}

	void TransformComponent::Translate(const float x, const float y, const float z) noexcept {
		Translate({ x, y, z });
	}

	void TransformComponent::Rotate(const glm::vec3 rotationToAdd) noexcept {
		if (isStatic) return;
		if (constexpr float epsilon = std::numeric_limits<float>::epsilon(); glm::length2(rotationToAdd) < epsilon * epsilon) return;

		localRotation *= glm::quat(glm::radians(rotationToAdd));
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	void TransformComponent::Rotate(const glm::quat rotationToAdd) noexcept {
		if (isStatic) return;

		localRotation *= rotationToAdd;
		RecalculateLocalMatrixAndInvokeOnModified();
	}

	glm::vec3 TransformComponent::GetRight() const noexcept {
		return GetRotation() * glm::vec3(1, 0, 0);
	}

	glm::vec3 TransformComponent::GetUp() const noexcept {
		return GetRotation() * glm::vec3(0, 1, 0);
	}

	glm::vec3 TransformComponent::GetForward() const noexcept {
		return GetRotation() * glm::vec3(0, 0, 1);
	}

	glm::vec3 TransformComponent::GetLocalRight() const noexcept {
		return localRotation * glm::vec3(1, 0, 0);
	}

	glm::vec3 TransformComponent::GetLocalUp() const noexcept {
		return localRotation * glm::vec3(0, 1, 0);
	}

	glm::vec3 TransformComponent::GetLocalForward() const noexcept {
		return localRotation * glm::vec3(0, 0, 1);
	}

	void TransformComponent::LookAt(const std::shared_ptr<TransformComponent>& target) noexcept {
		if (isStatic) return;
		if (!target) {
			DEBUG_ERROR("Cannot look at a null target!");
			return;
		}

		LookAt(target->GetPosition());
	}

	void TransformComponent::LookAt(const glm::vec3 targetPosition) noexcept {
		if (isStatic) return;

		localRotation = glm::quatLookAt(GetPosition() - targetPosition, GetUp());
		RecalculateLocalMatrixAndInvokeOnModified();
		//localMatrix = glm::lookAt(GetLocalPosition(), parent ? parent->GetPosition() - targetPosition : targetPosition, GetLocalUp());
		//DecomposeLocalMatrix();
	}

	void TransformComponent::DecomposeLocalMatrix() noexcept {
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(localMatrix, localScale, localRotation, localPosition, skew, perspective);

		OnModifiedEvent(std::static_pointer_cast<TransformComponent>(shared_from_this()));
	}

	void TransformComponent::RecalculateLocalMatrix() noexcept {
		const glm::mat4 rotationMatrix = glm::toMat4(localRotation);

		localMatrix = glm::scale(rotationMatrix, localScale);
		localMatrix[3][0] = localPosition.x;
		localMatrix[3][1] = localPosition.y;
		localMatrix[3][2] = localPosition.z;
	}

	void TransformComponent::RecalculateLocalMatrixAndInvokeOnModified() noexcept {
		RecalculateLocalMatrix();
		OnModifiedEvent(std::static_pointer_cast<TransformComponent>(shared_from_this()));
	}
}