#include "Engine/Utility/Time.hpp"
#include "Engine/engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"

namespace Engine
{
	float beginTime = 0;
	float endTime = 0;

	Time::Time() : deltaTime(0)
	{
		previousFramerates.resize(maxIterations);
	}

	float Time::GetDeltaTime() const
	{
		return deltaTime;
	}

	int Time::GetMaxIterations() const
	{
		return maxIterations;
	}

	eastl::vector<float> Time::GetPreviousFramerates() const
	{
		return previousFramerates;
	}

	void Time::OnUpdateBegin()
	{
		beginTime = float(glfwGetTime());
	}

	void Time::OnUpdateEnd()
	{
		endTime = float(glfwGetTime());
		deltaTime = beginTime > 0.0 ? float(endTime - beginTime) : float(1.0f / 60.0f);
		previousFramerates[iterations + 1 >= maxIterations ? 0 : iterations + 1] = ImGui::GetIO().Framerate;
		iterations++;
	}
} //namespace Engine