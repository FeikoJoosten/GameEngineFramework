#include <Engine/Utility/Logging.hpp>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#ifdef COUT_LOGGING
#include <iostream>
#endif

std::ofstream log_file;

void doDebug(eastl::string Type, eastl::string debugClass, eastl::string function, eastl::string value)
{
	if (log_file.is_open() == false)
	{
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
	eastl::string timeString = eastl::string(str).substr(0, 24);
	eastl::string logString = "[" + timeString + "] " + Type + " [" + debugClass + "::" + function + "]: " + value + "\n";
	log_file << logString.c_str();
#ifdef COUT_LOGGING
	std::cout << logString.c_str() << std::endl;
#endif
}