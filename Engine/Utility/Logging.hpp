#pragma once
#include "Engine/api.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/string.h>

#ifdef _DEBUG
#define COUT_LOGGING
#endif

// comment to disable info, warning or debug.
#define DEBUG_INFO_ENABLED
#define DEBUG_WARNING_ENABLED
#define DEBUG_ERROR_ENABLED

#define log_path "engine.log"

void ENGINE_API doDebug(eastl::string Type, eastl::string debugClass, eastl::string function, eastl::string value);

/* example: debug_warning("ResourceManager","GetFile()","File not found"); */

/*! \brief This functions logs a debug error to the log and console.
*	This does not halt the program!
*
*	This macro requres three parameters-
*	debug_class -> the class you're working on.
*	function -> the function you're working in.
*	value -> the message you want to pass on.
*
*/
#if defined(DEBUG_ERROR_ENABLED)
#define debug_error(debug_class, function, value) doDebug("[ERROR]", debug_class, function, value);
#else
#define debug_error(debug_class, function, value) {}
#endif


/*! \brief This functions logs a debug warning to the log and console.
*
*	This macro requres three parameters-
*	debug_class -> the class you're working on.
*	function -> the function you're working in.
*	value -> the message you want to pass on.
*/

#if defined(DEBUG_WARNING_ENABLED)
#define debug_warning(debug_class, function, value) doDebug("[WARNING]", debug_class, function, value);
#else
#define debug_warning(debug_class, function, value) {}
#endif

/*! \brief This functions logs a debug info to the log and console.
*	Use this for useless information.
*
*	This macro requres three parameters-
*	debug_class -> the class you're working on.
*	function -> the function you're working in.
*	value -> the message you want to pass on.
*
*/
#if defined(DEBUG_INFO_ENABLED)
#define debug_info(debug_class, function, value) doDebug("[INFO]", debug_class, function, value);
#else
#define debug_info(debug_class, function, value) {}
#endif