#pragma once

#include "Engine/Components/Component.hpp"
#include "Engine/Entity/Entity.hpp"
#include "Engine/Utility/Defines.hpp"
#include "Engine/Utility/Event.hpp"

#include <cereal/types/polymorphic.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine {
	class ENGINE_API TransformComponent final : public Component {
		friend cereal::access;
		template <class ComponentType>
		friend std::shared_ptr<ComponentType> Entity::AddComponent();

		glm::vec3 position {};
		glm::quat rotation {};
		glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
		bool isStatic = false;

		glm::mat4x4 modelMatrix {};

		TransformComponent();

	public:
		Sharp::Event<std::shared_ptr<TransformComponent>> OnModifiedEvent;

		virtual ~TransformComponent() override = default;
		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) noexcept = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) noexcept = delete;

		void SetPosition(glm::vec3 newPosition) noexcept;
		void SetPosition(float x, float y, float z) noexcept;
		[[nodiscard]] glm::vec3 GetPosition() const noexcept;

		void SetRotation(glm::quat newRotation) noexcept;
		void SetRotation(float x, float y, float z) noexcept;
		[[nodiscard]] glm::quat GetRotation() const noexcept;

		void SetPositionAndRotation(glm::vec3 newPosition, glm::quat newRotation);

		void SetScale(glm::vec3 newScale) noexcept;
		void SetScale(float x, float y, float z) noexcept;
		[[nodiscard]] glm::vec3 GetScale() const noexcept;

		void SetModelMatrix(glm::mat4x4 newModelMatrix) noexcept;
		void SetModelMatrix(float newModelMatrix[16]) noexcept;
		void SetModelMatrix(glm::vec4 newModelMatrix[4]) noexcept;
		[[nodiscard]] glm::mat4x4 GetModelMatrix() const noexcept;

		void SetIsStatic(bool newIsStatic) noexcept;
		[[nodiscard]] bool GetIsStatic() const noexcept;

		void Translate(glm::vec3 positionToAdd) noexcept;
		void Translate(float x) noexcept;
		void Translate(float x, float y) noexcept;
		void Translate(float x, float y, float z) noexcept;

		void Rotate(glm::quat rotationToAdd) noexcept;
		void AddRotation(float x) noexcept;
		void AddRotation(float x, float y) noexcept;
		void AddRotation(float x, float y, float z) noexcept;

		void AddScale(glm::vec3 scaleToAdd) noexcept;
		void AddScale(float x) noexcept;
		void AddScale(float x, float y) noexcept;
		void AddScale(float x, float y, float z) noexcept;

		[[nodiscard]] glm::vec3 GetRight() const noexcept;
		[[nodiscard]] glm::vec3 GetUp() const noexcept;
		[[nodiscard]] glm::vec3 GetForward() const noexcept;

		void LookAt(const TransformComponent& target) noexcept;
		void LookAt(glm::vec3 targetPosition) noexcept;

	private:

		void DecomposeModelMatrix() noexcept;

		void RecalculateModelMatrix() noexcept;

		void RecalculateModelMatrixAndInvokedOnModified() noexcept;

		template<class Archive>
		void Save(Archive& archive) const;

		template<class Archive>
		void Load(Archive& archive);
	};

	template <class Archive>
	void TransformComponent::Save(Archive& archive) const {
		archive(
			cereal::make_nvp("Component", cereal::virtual_base_class<Component>(this)),
			CEREAL_NVP(position),
			CEREAL_NVP(rotation),
			CEREAL_NVP(scale),
			CEREAL_NVP(isStatic)
		);
	}

	template <class Archive>
	void TransformComponent::Load(Archive& archive) {
		archive(
			cereal::make_nvp("Component", cereal::virtual_base_class<Component>(this)),
			CEREAL_NVP(position),
			CEREAL_NVP(rotation),
			CEREAL_NVP(scale),
			CEREAL_NVP(isStatic)
		);

		RecalculateModelMatrix();
	}

} // namespace Engine

CEREAL_REGISTER_TYPE(Engine::TransformComponent);
