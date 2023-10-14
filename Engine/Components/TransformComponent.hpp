#pragma once

#include "Engine/Components/Component.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Utility/Event.hpp"

#include <cereal/types/base_class.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine {
	class ENGINE_API TransformComponent final : public Component {
		friend class cereal::access;
		template <class ComponentType>
		friend std::shared_ptr<ComponentType> Entity::AddComponent();

		std::shared_ptr<TransformComponent> parent {};
		glm::vec3 localPosition {};
		glm::quat localRotation {};
		glm::vec3 localScale = glm::vec3(1.f, 1.f, 1.f);
		bool isStatic = false;

		glm::mat4x4 localMatrix {};

		TransformComponent();

	public:
		Sharp::Event<std::shared_ptr<TransformComponent>> OnModifiedEvent;

		virtual ~TransformComponent() override = default;
		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) noexcept = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) noexcept = delete;

		void SetParent(const std::shared_ptr<TransformComponent>& newParent, bool keepWorldPosition = true);
		[[nodiscard]] std::shared_ptr<TransformComponent> GetParent();

		void SetPosition(glm::vec3 newPosition) noexcept;
		void SetPosition(float x, float y, float z) noexcept;
		[[nodiscard]] glm::vec3 GetPosition() const noexcept;

		void SetLocalPosition(glm::vec3 newPosition) noexcept;
		void SetLocalPosition(float x, float y, float z) noexcept;
		[[nodiscard]] glm::vec3 GetLocalPosition() const noexcept;

		void SetRotation(glm::quat newRotation) noexcept;
		void SetRotation(float x, float y, float z) noexcept;
		[[nodiscard]] glm::quat GetRotation() const noexcept;

		void SetLocalRotation(glm::quat newRotation) noexcept;
		void SetLocalRotation(float x, float y, float z) noexcept;
		[[nodiscard]] glm::quat GetLocalRotation() const noexcept;

		void SetPositionAndRotation(glm::vec3 newPosition, glm::vec3 newRotation);
		void SetPositionAndRotation(glm::vec3 newPosition, glm::quat newRotation);

		void SetLocalPositionAndLocalRotation(glm::vec3 newPosition, glm::vec3 newRotation);
		void SetLocalPositionAndLocalRotation(glm::vec3 newPosition, glm::quat newRotation);

		void SetScale(glm::vec3 newScale) noexcept;
		void SetScale(float x, float y, float z) noexcept;
		[[nodiscard]] glm::vec3 GetScale() const noexcept;

		void SetLocalScale(glm::vec3 newScale) noexcept;
		void SetLocalScale(float x, float y, float z) noexcept;
		[[nodiscard]] glm::vec3 GetLocalScale() const noexcept;

		void SetMatrix(glm::mat4x4 newMatrix) noexcept;
		[[nodiscard]] glm::mat4x4 GetMatrix() const noexcept;

		void SetLocalMatrix(glm::mat4x4 newMatrix) noexcept;
		[[nodiscard]] glm::mat4x4 GetLocalMatrix() const noexcept;

		void SetIsStatic(bool newIsStatic) noexcept;
		[[nodiscard]] bool GetIsStatic() const noexcept;

		void Translate(glm::vec3 positionToAdd) noexcept;
		void Translate(float x) noexcept;
		void Translate(float x, float y) noexcept;
		void Translate(float x, float y, float z) noexcept;

		void Rotate(glm::vec3 rotationToAdd) noexcept;
		void Rotate(glm::quat rotationToAdd) noexcept;

		[[nodiscard]] glm::vec3 GetRight() const noexcept;
		[[nodiscard]] glm::vec3 GetUp() const noexcept;
		[[nodiscard]] glm::vec3 GetForward() const noexcept;

		[[nodiscard]] glm::vec3 GetLocalRight() const noexcept;
		[[nodiscard]] glm::vec3 GetLocalUp() const noexcept;
		[[nodiscard]] glm::vec3 GetLocalForward() const noexcept;

		void LookAt(const std::shared_ptr<TransformComponent>& target) noexcept;
		void LookAt(glm::vec3 targetPosition) noexcept;

	private:

		void DecomposeLocalMatrix() noexcept;

		void RecalculateLocalMatrix() noexcept;

		void RecalculateLocalMatrixAndInvokeOnModified() noexcept;

		template<class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void TransformComponent::Serialize(Archive& archive) {
		archive(
			cereal::make_nvp("Component", cereal::base_class<Component>(this)),
			CEREAL_NVP(parent),
			CEREAL_NVP(localPosition),
			CEREAL_NVP(localRotation),
			CEREAL_NVP(localScale),
			CEREAL_NVP(isStatic)
		);
	
		RecalculateLocalMatrix();
	}

}
