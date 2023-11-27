#pragma once
#include "Engine/Api/Api.hpp"

#include <string>

#ifndef COUT_LOGGING
#define COUT_LOGGING 1
#endif

// comment to disable info, warning or debug.
#ifndef DEBUG_DEBUG_ENABLED
#define DEBUG_DEBUG_ENABLED 1
#endif
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

#if DEBUG_ERROR_ENABLED
#define DEBUG_ERROR(value) DoDebug("[ERROR] ", __FILE__, __func__, __LINE__, value);
#else
#define DEBUG_ERROR(value) {}
#endif

#if DEBUG_WARNING_ENABLED
#define DEBUG_WARNING(value) DoDebug("[WARNING] ", __FILE__, __func__, __LINE__, value);
#else
#define DEBUG_WARNING(value) {}
#endif

#if DEBUG_INFO_ENABLED
#define DEBUG_INFO(value) DoDebug("[INFO] ", __FILE__, __func__, __LINE__, value);
#else
#define DEBUG_INFO(value) {}
#endif

#if DEBUG_DEBUG_ENABLED
#define DEBUG_DEBUG(value) DoDebug("[DEBUG] ", __FILE__, __func__, __LINE__, value);
#else
#define DEBUG_DEBUG(value) {}
#endif