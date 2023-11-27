#pragma once

#include "Engine/Api/Api.hpp"

#include <string>

namespace Engine {
	class ENGINE_API ShaderConstants {
	public:
		static inline const std::string MODEL_VIEW_PROJECTION = "u_mvp";
		static inline const std::string LIGHT_DIRECTION = "u_light_direction";
	};
}