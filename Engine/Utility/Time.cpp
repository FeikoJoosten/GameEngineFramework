#include "Engine/Utility/Time.hpp"

#include <GLFW/glfw3.h>

namespace Engine
{
	std::shared_ptr<Time> Time::Get() {
		return Engine::GetTime();
	}

	float Time::GetDeltaTime() const
	{
		return deltaTime;
	}

	int Time::GetNumberOfCompletedFrames() const {
		return numberOfCompletedFrames;
	}

	float Time::GetTimeAtStartOfFrame() const {
		return startOfCurrentFrame;
	}

	void Time::OnUpdateBegin()
	{
		startOfCurrentFrame = static_cast<float>(glfwGetTime());
	}

	void Time::OnUpdateEnd()
	{
		const float endTime = static_cast<float>(glfwGetTime());
		deltaTime = endTime - startOfCurrentFrame;
		numberOfCompletedFrames++;
	}
} //namespace Engine