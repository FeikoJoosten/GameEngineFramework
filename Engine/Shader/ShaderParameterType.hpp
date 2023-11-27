#pragma once

#include "Engine/Api/Api.hpp"

namespace Engine {
	enum class ENGINE_API ShaderParameterType {
		Float = 0,
		Int,
		Vector,
		Matrix,
		Texture
	};
}