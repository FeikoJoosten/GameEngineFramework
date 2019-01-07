#pragma once
#include "Engine/api.hpp"

namespace Engine
{
	/// <summary>
	/// This class provides random nummber generation of ints and floats withing the passed boundaries
	/// </summary>
	class ENGINE_API Random
	{
	public:

		/// <summary>
		/// Generate a number within the passed boundaries
		/// </summary>
		/// <param name="min">The minimum number to be generated</param>
		/// <param name="max">The maximum number to be generated</param>
		/// <returns>Returns a number from min to max</returns>
		int const& GenerateInt(int const& min, int const& max);

		/// <summary>
		/// Generate a number within the passed boundaries
		/// </summary>
		/// <param name="min">The minimum number to be generated</param>
		/// <param name="max">The maximum number to be generated</param>
		/// <returns>Returns a number from min to max</returns>
		float const& GenerateFloat(float const& min, float const& max);

	private:
		friend class Engine;
		/// <summary>
		/// Initializes the Seed based on the time here
		/// </summary>
		Random();
	public:
		~Random() = default;
	private:

		int lastResultInt;
		float lastResultFloat;
	};
} //namespace Engine

