#pragma once

#include "Engine/Api.hpp"
#include <cereal/cereal.hpp>

class ENGINE_API EngineSettings {
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

