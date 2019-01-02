#ifndef ENGINE_API_HPP
#define ENGINE_API_HPP

#if defined(ENGINE_DLL) || defined(ENGINE_AS_DLL)
	#ifdef ENGINE_AS_DLL
		#ifdef _WIN32
			#define ENGINE_API __declspec(dllimport)
		#else
			#define ENGINE_API
		#endif
		#define ENGINE_LOCAL
	#else
		#ifdef _WIN32
			#define ENGINE_API __declspec(dllexport)
			#define ENGINE_LOCAL
		#elif (defined(__GNUC__) && (__GNUC__ >= 4))
			#define ENGINE_API __attribute__ ((visibility("default")))
			#define ENGINE_LOCAL __attribute__ ((visibility("hidden")))
		#else
			#define ENGINE_API
			#define ENGINE_LOCAL
		#endif
	#endif
#else
	#define ENGINE_API
	#define ENGINE_LOCAL
#endif

#endif // ENGINE_API_HPP
