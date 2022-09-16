#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"

#include <memory>
#include <vector>

namespace Engine
{
	/// <summary>
	/// This object is used to retrieve information regarding the time. NOTE: This object can only be created by the Engine.
	/// </summary>
	class ENGINE_API Time
	{
		friend std::shared_ptr<Time> Engine::GetTime() noexcept;

		Time();
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
		/// This method is used to get the maximum amount of iterations for the previous frame rates.
		/// </summary>
		/// <returns>Returns the maximum amount of iterations as an int.</returns>
		[[nodiscard]] int GetMaxIterations() const;
		/// <summary>
		/// This method allows you to get a vector of floats from the previous frames their delta times.
		/// </summary>
		/// <returns>Returns the delta times from the previous frames as a vector of floats.</returns>
		[[nodiscard]] std::vector<float> GetPreviousFramerates() const;
	private:
		float deltaTime;
		std::vector<float> previousFramerates;
		int iterations = -1;
		int maxIterations = 100;

		friend class Application;
		static void OnUpdateBegin();
		void OnUpdateEnd();
	};
} //namespace Engine