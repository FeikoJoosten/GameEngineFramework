#include "Engine/Utility/Random.hpp"
#include "Engine/engine.hpp"
#include <cstdlib>
#include <ctime>

namespace Engine
{
	Random::Random() : lastResultInt(0), lastResultFloat(0.0f)
	{
		//Use current time as seed
		srand(unsigned int(time(nullptr)));
	}

	int const& Random::GenerateInt(int const& min, int const& max)
	{
		//Generate a number with the passed min and max flipped if the passed min is bigger than max
		if(min > max)
		{
			debug_warning("Random", "GenerateInt", "Passed Min value is larger than passed Max, returning GenerateInt() with passed Min as Max and passed Max as Min.");
			return GenerateInt(max, min);
		}
		
		//Return the passed min if the passed min and max are the same
		if (min == max)
		{
			debug_warning("Random", "GenerateInt", "Min and Max values passed have the same value, returning passed Min value.");
			return min;
		}

		//Generate the result and store it
		lastResultInt = rand() % (max + 1 - min) + min;

		//Return the result
		return lastResultInt;
	}

	float const& Random::GenerateFloat(float const& min, float const& max)
	{
		//Generate a number with the passed min and max flipped if the passed min is bigger than max
		if (min > max)
		{
			debug_warning("Random", "GenerateFloat", "Passed Min value is larger than passed Max, returning GenerateInt() with passed Min as Max and passed Max as Min.");
			return GenerateFloat(max, min);
		}

		//Return the passed min if the passed min and max are the same
		if (min == max)
		{
			debug_warning("Random", "GenerateFloat", "Min and Max values passed have the same value, returning passed Min value.");
			return min;
		}

		//Generate the result and store it
		const float shifter = 1000.0f;
		lastResultFloat = float(rand() % int(floorf((max - min) * shifter))) / shifter + min ;

		//Return the result
		return lastResultFloat;
	}
} //namespace Engine
