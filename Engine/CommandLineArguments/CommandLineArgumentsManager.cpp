#include "CommandLineArgumentsManager.hpp"

#include <algorithm>

namespace Engine {
	CommandLineArgumentsManager::CommandLineArgumentsManager(std::vector<std::string> commandLineArguments) :
		commandlineArguments(std::move(commandLineArguments)) {

	}

	bool CommandLineArgumentsManager::IsCommandLineArgumentDefined(const std::string& commandLineArgument) const {
		return std::any_of(commandlineArguments.begin(), commandlineArguments.end(), [commandLineArgument](const std::string& argument) {
			return argument == commandLineArgument;
			});
	}

	void CommandLineArgumentsManager::AddCommandLineArgument(std::string commandLineArgument) {
		if (IsCommandLineArgumentDefined(commandLineArgument)) return;

		commandlineArguments.push_back(std::move(commandLineArgument));
	}

	std::vector<std::string> CommandLineArgumentsManager::GetAllCommandLineArguments() const noexcept {
		return commandlineArguments;
	}
}
