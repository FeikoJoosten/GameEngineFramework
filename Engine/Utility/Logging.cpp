#include "Engine/Utility/Logging.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

static bool firstWrite = true;
static inline char* logPath;

constexpr bool IsPathSep(const char c) {
	return c == '/' || c == '\\';
}

constexpr const char* StripPath(const char* path) {
	const char* lastname = path;
	for (const char* p = path; *p; ++p) {
		if (IsPathSep(*p) && *(p + 1)) lastname = p + 1;
	}
	return lastname;
}

struct BasenameImpl {
	constexpr BasenameImpl(const char* begin, const char* end) : begin(begin), end(end) {}

	void Write(std::ostream& os) const {
		os.write(begin, end - begin);
	}

	const char* const begin;
	const char* const end;
};

inline std::ostream& operator<<(std::ostream& os, const BasenameImpl& bi) {
	bi.Write(os);
	return os;
}

constexpr const char* LastDotOf(const char* p) {
	const char* lastDot = nullptr;
	for (; *p; ++p) {
		if (*p == '.')
			lastDot = p;
	}
	return lastDot ? lastDot : p;
}

void DoDebug(const char* debugLevel, const char* classPath, const char* function, const int lineNumber, const std::string& value) {
		const char* classFileName = StripPath(classPath);
	const BasenameImpl classType = BasenameImpl(classFileName, LastDotOf(classFileName));
	const __time64_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::ostringstream outputStream;

	outputStream
#if LOGGING_INCLUDE_DATE
		<< "[" << std::put_time(gmtime(&currentTime), LOGGING_DATE_TIME_FORMAT) << "] "
#endif
#if LOGGING_INCLUDE_DEBUG_LEVEL
		<< debugLevel
#endif
		<< "["
#if LOGGING_INCLUDE_CLASS_TYPE
		<< classType << "::"
#endif
		<< function << "]@" << lineNumber << " " << value << std::endl;
	const std::string outputString = outputStream.str();

	if (firstWrite) {
		std::string projectRoot = Engine::AssetManager::GetProjectRoot();
		std::string fullLogPath = projectRoot + LOG_PATH;
		logPath = new char[fullLogPath.size()]();
		strcpy(logPath, fullLogPath.c_str());
		if (std::filesystem::exists(logPath))
			std::filesystem::rename(logPath, fullLogPath + ".old");
		firstWrite = false;
	}

	if (logPath) {
		std::ofstream logFile;
		logFile.open(logPath, std::ios_base::app);
		logFile << outputString;
		logFile.close();
	}
#if COUT_LOGGING
	std::cout << outputString;
#endif
}