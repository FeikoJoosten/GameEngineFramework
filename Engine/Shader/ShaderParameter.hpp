#pragma once

#include "Engine/Api/Api.hpp"
#include "Engine/AssetManagement/AssetReference.hpp"
#include "Engine/Shader/ShaderParameterType.hpp"
#include "Engine/Texture/Texture.hpp"

#include <cereal/access.hpp>
#include <cereal/cereal.hpp>

namespace Engine {
	class ENGINE_API ShaderParameter final : public std::enable_shared_from_this<ShaderParameter> {
		friend cereal::access;
		friend class Shader;

		std::string name {};
		ShaderParameterType type {};
		float floatValue = 0;
		int intValue = 0;
		glm::vec4 vectorValue {};
		glm::mat4x4 matrixValue {};
		AssetReference<Texture> textureValue {};
		bool isGlobal {};

	public:
		Sharp::Event<const std::shared_ptr<ShaderParameter>&> OnParameterValueChangedEvent;

	private:
		ShaderParameter() = default;

	public:
		~ShaderParameter() = default;
		ShaderParameter(const ShaderParameter& other) = delete;
		ShaderParameter(ShaderParameter&& other) noexcept = delete;
		ShaderParameter& operator=(const ShaderParameter& other) = delete;
		ShaderParameter& operator=(ShaderParameter&& other) noexcept = delete;

		void SetValue(float newValue);
		void SetValue(int newValue);
		void SetValue(glm::vec2 newValue);
		void SetValue(glm::vec3 newValue);
		void SetValue(glm::vec4 newValue);
		void SetValue(glm::mat4x4 newValue);
		void SetValue(const std::shared_ptr<Texture>& newValue);

		[[nodiscard]] const std::string& GetName() const;
		[[nodiscard]] ShaderParameterType GetType() const;
		[[nodiscard]] float GetFloatValue() const;
		[[nodiscard]] int GetIntValue() const;
		[[nodiscard]] glm::vec4 GetVectorValue() const;
		[[nodiscard]] glm::mat4x4 GetMatrixValue() const;
		[[nodiscard]] std::shared_ptr<Texture> GetTextureValue() const;

	private:

		template <class Archive>
		void Serialize(Archive& archive);
	};

	template <class Archive>
	void ShaderParameter::Serialize(Archive& archive) {
		archive(
			CEREAL_NVP(name),
			CEREAL_NVP(type),
			CEREAL_NVP(floatValue),
			CEREAL_NVP(intValue),
			CEREAL_NVP(vectorValue),
			CEREAL_NVP(matrixValue),
			CEREAL_NVP(textureValue),
			CEREAL_NVP(isGlobal)
		);
	}
}
