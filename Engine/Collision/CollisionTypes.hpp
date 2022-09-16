#pragma once

/// <summary>
/// Enum for available collision shapes
/// </summary>
enum class CollisionShapeType
{
	Circle,
	Box
};

/// <summary>
/// Bitmask Enum that allows for layered collision checking
/// </summary>
enum class CollisionLayer
{
	None			= 0,
	Solid			= 1 << 0,	// 1
	Detection		= 1 << 1,	// 2
};