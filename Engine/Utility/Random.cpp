#include "Engine/Utility/Logging.hpp"
#include "Engine/Utility/Random.hpp"

namespace Engine {
	int Random::GenerateInt(const int& min, const int& max) {
		//Generate a number with the passed min and max flipped if the passed min is bigger than max
		if (min > max) {
			DEBUG_WARNING("Passed Min value is larger than passed Max, returning GenerateInt() with passed Min as Max and passed Max as Min.")
			return GenerateInt(max, min);
		}

		//Return the passed min if the passed min and max are the same
		if (min == max) {
			DEBUG_WARNING("Min and Max values passed have the same value, returning passed Min value.")
			return min;
		}

		//Return the result
		return rand() % (max + min) + min;
	}

	float Random::GenerateFloat(const float& min, const float& max) {
		//Generate a number with the passed min and max flipped if the passed min is bigger than max
		if (min > max) {
			DEBUG_WARNING("Passed Min value is larger than passed Max, returning GenerateInt() with passed Min as Max and passed Max as Min.")
			return GenerateFloat(max, min);
		}

		//Return the passed min if the passed min and max are the pretty much same
		if (min - max <= 0.000001f) {
			DEBUG_WARNING("Min and Max values passed have the same value, returning passed Min value.")
			return min;
		}

		//Generate the result and store it
		constexpr float shifter = 1000.0f;

		//Return the result
		return static_cast<float>(rand() % static_cast<int>(floorf((max - min) * shifter))) / shifter + min;
	}
} //namespace Engine
