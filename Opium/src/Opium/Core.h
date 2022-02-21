#pragma once

// We understand what is the platform that our
// application is working
#if defined _WIN32 || defined __CYGWIN__
	#define OP_PLATFORM_WINDOWS
#endif

// According to the platform and whether we are compiling
// application or opium engine, we define OPIUM_API being
// import or export
#ifdef OP_PLATFORM_WINDOWS
	#ifdef OP_BUILD_DLL
		#ifdef __GNUC__
			#define OPIUM_API __attribute__ ((dllexport))
		#else
			#define OPIUM_API __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define OPIUM_API __attribute__ ((dllimport))
		#else
			#define OPIUM_API __declspec(dllimport)
		#endif
	#endif
	#define NOT_EXPORTED
#else
	#if __GNUC__ >= 4
		#define OPIUM_API __attribute__ ((visibility ("default")))
		#define NOT_EXPORTED __attribute__ ((visibility ("hidden")))
	#else
		#define OPIUM_API
		#define NOT_EXPORTED
	#endif
#endif

// This is for event categories
#define BITFIELD(x) (1 << x)

#define OP_BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)