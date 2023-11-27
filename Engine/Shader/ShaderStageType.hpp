#pragma once

#include "Engine/Api/Api.hpp"

namespace Engine {
	enum class ENGINE_API ShaderStageType {
		Vertex = 0,
		Fragment,
		Geometry,
		Compute,
		// What about any other shader types? Should this allow for custom types somehow? If so, how are those then handles by each graphics platform during setup?

	};
}