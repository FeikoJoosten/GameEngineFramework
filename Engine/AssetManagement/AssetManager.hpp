#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"

#include <memory>
#include <string>

namespace Engine {
	class ENGINE_API AssetManager {
		friend std::shared_ptr<AssetManager> Engine::GetAssetManager() noexcept;

		AssetManager() = default;

	public:
		AssetManager(const AssetManager& other) = delete;
		AssetManager(AssetManager&& other) noexcept = delete;
		~AssetManager() = default;

		AssetManager& operator=(const AssetManager& other) = delete;
		AssetManager& operator=(AssetManager&& other) noexcept = delete;

		static std::shared_ptr<AssetManager> Get();

		const std::string& GetProjectRoot();

		static std::string GetDirectoryFromPath(const std::string& path);

		// TODO: Make this private
		void SetExecutablePath(const std::string& executablePath);

	private:
		std::string fullExecutablePath {};
		std::string fullExecutableDirectory {};

	};
}
