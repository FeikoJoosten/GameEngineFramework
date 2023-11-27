#include "Engine/Shader/ShaderParameter.hpp"

namespace Engine {
	void ShaderParameter::SetValue(const float newValue) {
		if (type != ShaderParameterType::Float) return;
		if (floatValue - newValue < std::numeric_limits<float>::epsilon()) return;
		floatValue = newValue;
		OnParameterValueChangedEvent(shared_from_this());
	}

	void ShaderParameter::SetValue(const int newValue) {
		if (type != ShaderParameterType::Int) return;
		if (intValue == newValue) return;
		intValue = newValue;
		OnParameterValueChangedEvent(shared_from_this());
	}

	void ShaderParameter::SetValue(const glm::vec2 newValue) {
		SetValue(glm::vec4(newValue, 0, 0));
	}

	void ShaderParameter::SetValue(const glm::vec3 newValue) {
		SetValue(glm::vec4(newValue, 0));
	}

	void ShaderParameter::SetValue(const glm::vec4 newValue) {
		if (type != ShaderParameterType::Vector) return;
		if (vectorValue == newValue) return;
		vectorValue = newValue;
		OnParameterValueChangedEvent(shared_from_this());
	}

	void ShaderParameter::SetValue(const glm::mat4x4 newValue) {
		if (type != ShaderParameterType::Matrix) return;
		if (matrixValue == newValue) return;
		matrixValue = newValue;
		OnParameterValueChangedEvent(shared_from_this());
	}

	void ShaderParameter::SetValue(const std::shared_ptr<Texture>& newValue) {
		if (type != ShaderParameterType::Texture) return;
		if (textureValue == newValue) return;
		textureValue = newValue;
		OnParameterValueChangedEvent(shared_from_this());
	}

	const std::string& ShaderParameter::GetName() const {
		return name;
	}

	ShaderParameterType ShaderParameter::GetType() const {
		return type;
	}

	float ShaderParameter::GetFloatValue() const {
		if (type != ShaderParameterType::Float) return 0;
		return floatValue;
	}

	int ShaderParameter::GetIntValue() const {
		if (type != ShaderParameterType::Int) return 0;
		return intValue;
	}

	glm::vec4 ShaderParameter::GetVectorValue() const {
		if (type != ShaderParameterType::Vector) return {};
		return vectorValue;
	}

	glm::mat4x4 ShaderParameter::GetMatrixValue() const {
		if (type != ShaderParameterType::Matrix) return {};
		return matrixValue;
	}

	std::shared_ptr<Texture> ShaderParameter::GetTextureValue() const {
		if (type != ShaderParameterType::Texture) return nullptr;
		return textureValue;
	}
}
