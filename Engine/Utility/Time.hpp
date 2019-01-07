#pragma once

#include "Engine/api.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>

namespace Engine
{
	/// <summary>
	/// This object is used to retrieve information regarding the time. NOTE: This object can only be created by the Engine.
	/// </summary>
	class ENGINE_API Time
	{
		friend class Engine;

		Time();
	public:
		~Time() = default;

		/// <summary>
		/// This method allows you to get the current deltaTime.
		/// </summary>
		/// <returns>Returns the current delta time as a float.</returns>
		float GetDeltaTime() const;
		/// <summary>
		/// This method is used to get the maximum amount of iterations for the previous frame rates.
		/// </summary>
		/// <returns>Returns the maximum amount of iterations as an int.</returns>
		int GetMaxIterations() const;
		/// <summary>
		/// This method allows you to get a vector of floats from the previous frames their delta times.
		/// </summary>
		/// <returns>Returns the delta times from the previous frames as a vector of floats.</returns>
		eastl::vector<float> GetPreviousFramerates() const;
	private:
		float deltaTime;
		eastl::vector<float> previousFramerates;
		int iterations = -1;
		int maxIterations = 100;

		friend class Engine;
		static void OnUpdateBegin();
		void OnUpdateEnd();
	};
} //namespace Engine