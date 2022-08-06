#pragma once
#include <memory>

// 3D RENDERER CONSTANTS
#define MAX_DIR_LIGHTS 2
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADE_SIZE 15
#define MAX_POINT_LIGHTS 10
// --------------------

// We understand what is the platform that our
// application is working
#if defined _WIN32 || defined __CYGWIN__
	#define OP_PLATFORM_WINDOWS
#endif

// According to the platform and whether we are compiling
// application or opium engine, we define OPIUM_API being
// import or export
#ifdef OP_PLATFORM_WINDOWS
	#if OP_DYNAMIC_LINK
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
		#define OPIUM_API
		#define NOT_EXPORTED
	#endif
#else
	#if OP_DYNAMIC_LINK
		#if __GNUC__ >= 4
			#define OPIUM_API __attribute__ ((visibility ("default")))
			#define NOT_EXPORTED __attribute__ ((visibility ("hidden")))
		#else
			#define OPIUM_API
			#define NOT_EXPORTED
		#endif
	#else
		#define OPIUM_API
		#define NOT_EXPORTED
	#endif
#endif

// This is for event categories
#define BITFIELD(x) (1 << x)

// #define OP_BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

#define OP_BIND_EVENT_FUNCTION(x) [this](auto&&... args) -> decltype(auto) { return this->x(std::forward<decltype(args)>(args)...); }

#define OP_BIND_FUNCTION(x) [this](auto&&... args) -> decltype(auto) { return this->x(std::forward<decltype(args)>(args)...); }
namespace OP
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}