#pragma once

#include "Engine/api.hpp"

class ENGINE_API Component
{
public:
	explicit Component() = default;
	virtual ~Component() = default;

	virtual void Update();
};
