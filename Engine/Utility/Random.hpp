#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"

#include <random>

namespace Engine
{
	/// <summary>
	/// This class provides random number generation of integers and floats withing the passed boundaries
	/// </summary>
	class ENGINE_API Random
	{
		friend std::shared_ptr<Random> Engine::GetRandom() noexcept;
		Random() = default;
	public:
		~Random() = default;
		Random(const Random& other) = delete;
		Random(Random&& other) noexcept = delete;

		Random& operator=(const Random& other) = delete;
		Random& operator=(Random&& other) noexcept = delete;

		/// <summary>
		/// Generate a number within the passed boundaries
		/// </summary>
		/// <param name="min">The minimum number to be generated</param>
		/// <param name="max">The maximum number to be generated</param>
		/// <returns>Returns a number from min to max</returns>
		static int GenerateInt(const int& min, const int& max);

		/// <summary>
		/// Generate a number within the passed boundaries
		/// </summary>
		/// <param name="min">The minimum number to be generated</param>
		/// <param name="max">The maximum number to be generated</param>
		/// <returns>Returns a number from min to max</returns>
		static float GenerateFloat(const float& min, const float& max);
	};
} //namespace Engine

