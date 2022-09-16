#pragma once

#include "Engine/Api.hpp"

namespace Engine {
	class ENGINE_API Application {
	public:
		static bool GetIsPlaying();

		static void SetIsPlaying(bool newIsPlaying);

		/// <summary>
		/// The general update method of the entire engine system. 
		/// Call this from your main loop.
		/// </summary>
		static void Update();

		/// <summary>
		/// This method shuts down the entire engine for you. 
		/// Call this method after your main loop is finished.
		/// </summary>
		static void Quit();
	private:

		static void Render();
	};
} // namespace Engine

