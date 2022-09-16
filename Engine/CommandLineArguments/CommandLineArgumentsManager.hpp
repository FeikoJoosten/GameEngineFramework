#pragma once

#include "Engine/Api.hpp"
#include "Engine/Engine.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Engine {
	class ENGINE_API CommandLineArgumentsManager {
		static friend std::shared_ptr<CommandLineArgumentsManager> Engine::GetOrCreateCommandLineArgumentsManager(std::vector<std::string> commandLineArguments) noexcept;

		explicit CommandLineArgumentsManager(std::vector<std::string> commandLineArguments);

	public:
		CommandLineArgumentsManager() = delete;
		CommandLineArgumentsManager(const CommandLineArgumentsManager& other) = delete;
		CommandLineArgumentsManager(CommandLineArgumentsManager&& other) noexcept = delete;
		~CommandLineArgumentsManager() = default;

		CommandLineArgumentsManager& operator=(const CommandLineArgumentsManager& other) = delete;
		CommandLineArgumentsManager& operator=(CommandLineArgumentsManager&& other) noexcept = delete;

		[[nodiscard]] bool IsCommandLineArgumentDefined(const std::string& commandLineArgument) const;

		void AddCommandLineArgument(std::string commandLineArgument);

		[[nodiscard]] std::vector<std::string> GetAllCommandLineArguments() const noexcept;

	private:
		std::vector<std::string> commandlineArguments;
	};
}
