#pragma once

#include "Engine/AssetManagement/Asset.hpp"
#include "Engine/Texture/Texture.hpp"

namespace Engine {

	class ENGINE_API Material : public Asset
	{
		explicit Material() = default;
	public:
		virtual ~Material() override = default;
		Material(const Material& other) = delete;
		Material(Material&& other) noexcept = delete;

		Material& operator=(const Material& other) = delete;
		Material& operator=(Material&& other) noexcept = delete;
	};

}