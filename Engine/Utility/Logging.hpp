#pragma once
#include "Engine/Api/Api.hpp"

#include <string>

#ifndef COUT_LOGGING
#define COUT_LOGGING 1
#endif

// comment to disable info, warning or debug.
#ifndef DEBUG_INFO_ENABLED
#define DEBUG_INFO_ENABLED 1
#endif
#ifndef DEBUG_WARNING_ENABLED
#define DEBUG_WARNING_ENABLED 1
#endif
#ifndef DEBUG_ERROR_ENABLED
#define DEBUG_ERROR_ENABLED 1
#endif

#ifndef LOG_PATH
#define LOG_PATH "engine.log"
#endif

#ifndef LOGGING_INCLUDE_DATE
#define LOGGING_INCLUDE_DATE 1

#ifndef LOGGING_DATE_TIME_FORMAT
#define LOGGING_DATE_TIME_FORMAT "%c"
#endif
#endif

#ifndef LOGGING_INCLUDE_DEBUG_LEVEL
#define LOGGING_INCLUDE_DEBUG_LEVEL 1
#endif

#ifndef LOGGING_INCLUDE_CLASS_TYPE
#define LOGGING_INCLUDE_CLASS_TYPE 1
#endif

void ENGINE_API DoDebug(const char* debugLevel, const char* classPath, const char* function, int lineNumber, const std::string& value);

/* example: debug_warning("ResourceManager","GetFile()","File not found"); */

/*! \brief This functions logs a debug error to the log and console.
*	This does not halt the program!
*
*	This macro requires three parameters-
*	debug_class -> the class you're working on.
*	function -> the function you're working in.
*	value -> the message you want to pass on.
*
*/
#if DEBUG_ERROR_ENABLED
#define DEBUG_ERROR(value) DoDebug("[ERROR] ", __FILE__, __func__, __LINE__, value);
#else
#define DEBUG_ERROR(value) {}
#endif


/*! \brief This functions logs a debug warning to the log and console.
*
*	This macro requires three parameters-
*	debug_class -> the class you're working on.
*	function -> the function you're working in.
*	value -> the message you want to pass on.
*/

#if DEBUG_WARNING_ENABLED
#define DEBUG_WARNING(value) DoDebug("[WARNING] ", __FILE__, __func__, __LINE__, value);
#else
#define DEBUG_WARNING(value) {}
#endif

/*! \brief This functions logs a debug info to the log and console.
*	Use this for useless information.
*
*	This macro requires three parameters-
*	debug_class -> the class you're working on.
*	function -> the function you're working in.
*	value -> the message you want to pass on.
*
*/
#if DEBUG_INFO_ENABLED
#define DEBUG_INFO(value) DoDebug("[INFO] ", __FILE__, __func__, __LINE__, value);
#else
#define DEBUG_INFO(value) {}
#endif