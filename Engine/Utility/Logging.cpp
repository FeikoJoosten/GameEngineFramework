#include <Engine/Utility/Logging.hpp>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#ifdef COUT_LOGGING
#include <iostream>
#endif

std::ofstream log_file;

void doDebug(std::string Type, std::string debugClass, std::string function, std::string value)
{
	if (!log_file.is_open())
	{
		if (log_file.bad()) return;
		log_file.open(log_path);
		log_file.clear();
	}

	//get time
	struct tm buff;
	time_t ltime = time(nullptr);

	//prepare string buffer
	char str[26];

	//convert time to string
	localtime_s(&buff, &ltime);
	asctime_s(str, sizeof str, &buff);

	//pass on time string
	std::string timeString = std::string(str).substr(0, 24);
	std::string logString = "[" + timeString + "] " + Type + " [" + debugClass + "::" + function + "]: " + value + "\n";
	log_file << logString.c_str();
	log_file.close();
#ifdef COUT_LOGGING
	std::cout << logString.c_str() << std::endl;
#endif
}