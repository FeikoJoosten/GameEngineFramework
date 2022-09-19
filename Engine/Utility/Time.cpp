#include "Engine/Utility/Time.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	float beginTime = 0;
	float endTime = 0;

	Time::Time() : deltaTime(0)
	{
		previousFramerates.resize(maxIterations);
	}

	std::shared_ptr<Time> Time::Get() {
		return Engine::GetTime();
	}

	float Time::GetDeltaTime() const
	{
		return deltaTime;
	}

	int Time::GetMaxIterations() const
	{
		return maxIterations;
	}

	std::vector<float> Time::GetPreviousFramerates() const
	{
		return previousFramerates;
	}

	void Time::OnUpdateBegin()
	{
		beginTime = static_cast<float>(glfwGetTime());
	}

	void Time::OnUpdateEnd()
	{
		endTime = static_cast<float>(glfwGetTime());
		deltaTime = beginTime > 0.0f ? endTime - beginTime : 1.0f / 60.0f;
		if (++iterations >= maxIterations) iterations = 0;
		previousFramerates[iterations] = ImGui::GetIO().Framerate;
	}
} //namespace Engine