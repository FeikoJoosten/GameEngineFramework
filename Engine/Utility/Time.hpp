#pragma once

#include "Engine/api.hpp"
#include "ThirdParty/EASTL-master/include/EASTL/vector.h"

namespace Engine
{
	class ENGINE_API Time
	{
	public:
		Time();
		~Time();

		float GetDeltaTime();
		int GetMaxIterations();
		eastl::vector<float> GetPreviousFramerates();
	private:
		float deltaTime;
		eastl::vector<float> previousFramerates;
		int iterations = -1;
		int maxIterations = 100;

		friend class Engine;
		void OnUpdateBegin();
		void OnUpdateEnd();
	};
} //namespace Engine