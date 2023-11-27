#include "Engine/Material/Material.hpp"

CEREAL_REGISTER_TYPE(Engine::Material);

namespace Engine {
	std::shared_ptr<Shader> Material::GetShader() const {
		return shader;
	}

	void Material::SetShader(const xg::Guid& newShaderGuid) {
		if (shader == newShaderGuid) return;

		const std::shared_ptr<Shader> oldShader = shader;
		shader = newShaderGuid;
		OnShaderChangedEvent(std::static_pointer_cast<Material>(shared_from_this()), oldShader, shader);
	}

	void Material::SetShader(const std::shared_ptr<Shader>& newShader) {
		if (shader == newShader) return;

		const std::shared_ptr<Shader> oldShader = shader;
		shader = newShader;
		OnShaderChangedEvent(std::static_pointer_cast<Material>(shared_from_this()), oldShader, shader);
	}
}
