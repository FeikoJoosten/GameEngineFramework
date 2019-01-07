#pragma once

/// <summary>
/// Enum for available collision shapes
/// </summary>
enum class CollisionShapeType
{
	CIRCLE,
	BOX
};

/// <summary>
/// Bitmask Enum that allows for layered collision checking
/// </summary>
enum class CollisionLayer
{
	NONE			= 0,
	SOLID			= 1 << 0,	// 1
	DETECTION		= 1 << 1,	// 2
	RED_TEAM		= 1 << 2,	// 4
	BLUE_TEAM		= 1 << 3,	// 8
	DAMAGE			= 1 << 4	// etc
};