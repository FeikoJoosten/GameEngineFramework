#include "Engine/Components/TransformComponent.hpp"
#include <ThirdParty/glm/glm/gtc/matrix_transform.hpp>
#include <ThirdParty/glm/glm/gtx/euler_angles.hpp>

namespace Engine
{
	TransformComponent::TransformComponent() noexcept : TransformComponent(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), false)
	{
	}

	TransformComponent::TransformComponent(bool isStatic) noexcept : TransformComponent(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), isStatic)
	{
	}

	TransformComponent::TransformComponent(glm::vec3 position) noexcept : TransformComponent(position, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), false)
	{
	}

	TransformComponent::TransformComponent(glm::vec3 position, bool isStatic) noexcept : TransformComponent(position, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), isStatic)
	{
	}

	TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 rotation) noexcept : TransformComponent(position, rotation, glm::vec3(1, 1, 1), false)
	{
	}

	TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 rotation, bool isStatic) noexcept : TransformComponent(position, rotation, glm::vec3(1, 1, 1), isStatic)
	{
	}

	TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) noexcept : TransformComponent(position, rotation, scale, false)
	{
	}

	TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool isStatic) noexcept : position(position), rotation(rotation), scale(scale), isStatic(isStatic)
	{
		UpdateModelMatrix();
	}

	void TransformComponent::SetPosition(glm::vec3 position) noexcept
	{
		if (isStatic) return;

		this->position = position;
		shouldRecalculateModelMatrix = true;
	}

	void TransformComponent::SetPosition(float x, float y, float z) noexcept
	{
		SetPosition(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetPosition() noexcept
	{
		return position;
	}

	void TransformComponent::SetRotation(glm::vec3 rotation) noexcept
	{
		if (isStatic) return;

		this->rotation = rotation;
		shouldRecalculateModelMatrix = true;
	}

	void TransformComponent::SetRotation(float x, float y, float z) noexcept
	{
		SetRotation(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetRotation() noexcept
	{
		return rotation;
	}

	void TransformComponent::SetScale(glm::vec3 scale) noexcept
	{
		if (isStatic) return;

		this->scale = scale;
		shouldRecalculateModelMatrix = true;
	}

	void TransformComponent::SetScale(float x, float y, float z) noexcept
	{
		SetScale(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetScale() noexcept
	{
		return scale;
	}

	void TransformComponent::SetModelMatrix(glm::mat4x4 modelMatrix) noexcept
	{
		if (isStatic) return;

		this->modelMatrix = modelMatrix;
		shouldRecalculateModelMatrix = true;
	}

	void TransformComponent::SetModelMatrix(float modelMatrix[16]) noexcept
	{
		SetModelMatrix(glm::mat4x4(modelMatrix[0], modelMatrix[1], modelMatrix[2], modelMatrix[3],
			modelMatrix[4], modelMatrix[5], modelMatrix[6], modelMatrix[7],
			modelMatrix[8], modelMatrix[9], modelMatrix[10], modelMatrix[11],
			modelMatrix[12], modelMatrix[13], modelMatrix[14], modelMatrix[15]));
	}

	void TransformComponent::SetModelMatrix(glm::vec4 modelMatrix[4]) noexcept
	{
		SetModelMatrix(glm::mat4x4(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z, modelMatrix[0].w,
			modelMatrix[1].x, modelMatrix[1].y, modelMatrix[1].z, modelMatrix[1].w,
			modelMatrix[2].x, modelMatrix[2].y, modelMatrix[2].z, modelMatrix[2].w,
			modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z, modelMatrix[3].w));
	}

	glm::mat4x4 TransformComponent::GetModelMatrix() noexcept
	{
		return modelMatrix;
	}

	void TransformComponent::SetIsStatic(bool isStatic) noexcept
	{
		this->isStatic = isStatic;
	}

	bool TransformComponent::GetIsStatic() noexcept
	{
		return isStatic;
	}

	void TransformComponent::Translate(glm::vec3 positionToAdd) noexcept
	{
		if (isStatic) return;

		position += positionToAdd;
		shouldRecalculateModelMatrix = true;
	}

	void TransformComponent::Translate(float x) noexcept
	{
		Translate(glm::vec3(x, 0, 0));
	}

	void TransformComponent::Translate(float x, float y) noexcept
	{
		Translate(glm::vec3(x, y, 0));
	}

	void TransformComponent::Translate(float x, float y, float z) noexcept
	{
		Translate(glm::vec3(x, y, z));
	}

	void TransformComponent::AddRotate(glm::vec3 rotationToAdd) noexcept
	{
		if (isStatic) return;

		rotation += rotationToAdd;
	}

	void TransformComponent::AddRotate(float x) noexcept
	{
		AddRotate(glm::vec3(x, 0, 0));
	}

	void TransformComponent::AddRotate(float x, float y) noexcept
	{
		AddRotate(glm::vec3(x, y, 0));
	}

	void TransformComponent::AddRotate(float x, float y, float z) noexcept
	{
		AddRotate(glm::vec3(x, y, z));
	}

	void TransformComponent::AddScale(glm::vec3 scaleToAdd) noexcept
	{
		if (isStatic) return;

		scale += scaleToAdd;
		shouldRecalculateModelMatrix = true;
	}

	void TransformComponent::AddScale(float x) noexcept
	{
		AddScale(glm::vec3(x, 0, 0));
	}

	void TransformComponent::AddScale(float x, float y) noexcept
	{
		AddScale(glm::vec3(x, y, 0));
	}

	void TransformComponent::AddScale(float x, float y, float z) noexcept
	{
		AddScale(glm::vec3(x, y, z));
	}

	glm::vec3 TransformComponent::GetRight() noexcept
	{
		return glm::normalize(glm::vec3(modelMatrix[0]));
	}

	glm::vec3 TransformComponent::GetUp() noexcept
	{
		return glm::normalize(glm::vec3(modelMatrix[1]));
	}

	glm::vec3 TransformComponent::GetForward() noexcept
	{
		return glm::normalize(glm::vec3(modelMatrix[2]));
	}

	void TransformComponent::LookAt(TransformComponent target) noexcept
	{
		LookAt(target.position);
	}

	void TransformComponent::LookAt(glm::vec3 targetPosition) noexcept
	{
		if (isStatic) return;

		modelMatrix *= glm::lookAt(position, targetPosition, GetUp());
		shouldRecalculateModelMatrix = true;
	}

	void TransformComponent::RotateAround(float angle, glm::vec3 axis) noexcept
	{
		if (isStatic) return;

		glm::mat4x4 rotationMatrix;
		glm::rotate(rotationMatrix, angle, axis);
		modelMatrix *= rotationMatrix;

		shouldRecalculateModelMatrix = true;
	}

	void TransformComponent::Update()
	{
		if (shouldRecalculateModelMatrix == false) return;

		UpdateModelMatrix();
	}

	void TransformComponent::UpdateModelMatrix() noexcept
	{
		glm::mat4x4 rotationMatrix = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
		glm::mat4x4 rotationScaleMatrix = rotationMatrix * glm::scale(rotationMatrix, scale);
		rotationScaleMatrix[3] = glm::vec4(position, 1);

		modelMatrix = rotationScaleMatrix;
		shouldRecalculateModelMatrix = false;
	}

	bool TransformComponent::operator!=(TransformComponent& other)
	{
		if (position != other.position) return true;
		if (rotation != other.rotation) return true;
		if (scale != other.scale) return true;
		return false;
	}

	bool TransformComponent::operator==(TransformComponent& other)
	{
		if (position != other.position) return false;
		if (rotation != other.rotation) return false;
		if (scale != other.scale) return false;
		return true;
	}
} //namespace Engine