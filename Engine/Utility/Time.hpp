#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Application/Application.hpp"

#include <memory>

namespace Engine
{
	/// <summary>
	/// This object is used to retrieve information regarding the time. NOTE: This object can only be created by the Engine.
	/// </summary>
	class ENGINE_API Time final
	{
		friend std::shared_ptr<Time> Engine::GetTime() noexcept;
		friend void Application::Update();

		Time() = default;
	public:
		~Time() = default;
		Time(const Time& other) = delete;
		Time(Time&& other) noexcept = delete;

		Time& operator=(const Time& other) = delete;
		Time& operator=(Time&& other) noexcept = delete;

		static std::shared_ptr<Time> Get();

			/// <summary>
		/// This method allows you to get the current deltaTime.
		/// </summary>
		/// <returns>Returns the current delta time as a float.</returns>
		[[nodiscard]] float GetDeltaTime() const;

		/// <summary>
		/// This method is used to get the number of completed frames since launch.
		/// </summary>
		/// <returns>Returns the number of completed frames since launch as an int.</returns>
		[[nodiscard]] int GetNumberOfCompletedFrames() const;

		/// <summary>
		/// This method is used to get the start time of the current frame
		/// </summary>
		/// <returns>Returns the start time of the current frame in seconds.</returns>
		[[nodiscard]] float GetTimeAtStartOfFrame() const;

	private:
		float deltaTime = 0;
		float startOfCurrentFrame = 0;
		int numberOfCompletedFrames = 0;

		void OnUpdateBegin();
		void OnUpdateEnd();
	};
} //namespace Engine