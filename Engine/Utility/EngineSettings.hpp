#pragma once

#include "Engine/Api/Api.hpp"
#include <cereal/cereal.hpp>

class ENGINE_LOCAL EngineSettings {
public:
	float mouseWheelScrollSpeed = 5.f;

	template <class Archive>
	void Serialize(Archive& archive);
};

template <class Archive> void EngineSettings::Serialize(Archive& archive) {
	archive(
		CEREAL_NVP(mouseWheelScrollSpeed)
	);
}

