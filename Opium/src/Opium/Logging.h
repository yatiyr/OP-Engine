#pragma once

#include <Opium/Core.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

// Log macros for Engine
#define OP_ENGINE_FATAL(...) ::OP::Logging::GetEngineLogger()->critical(__VA_ARGS__);
#define OP_ENGINE_ERROR(...) ::OP::Logging::GetEngineLogger()->error(__VA_ARGS__);
#define OP_ENGINE_WARN(...)  ::OP::Logging::GetEngineLogger()->warn(__VA_ARGS__);
#define OP_ENGINE_INFO(...)  ::OP::Logging::GetEngineLogger()->info(__VA_ARGS__);
#define OP_ENGINE_TRACE(...) ::OP::Logging::GetEngineLogger()->trace(__VA_ARGS__);


// Log macros for assertions
#ifdef OP_ENABLE_ASSERTS
	#ifdef OP_PLATFORM_WINDOWS
		#define OP_ENGINE_ASSERT(x, ...) { if(!(x)) {OP_ENGINE_ERROR("Failed assertion: {0}", __VA_ARGS__); __debugbreak(); } }
	#elif defined __GNUC__
		#define OP_ENGINE_ASSERT(x, ...) { if(!(x)) {OP_ENGINE_ERROR("Failed assertion: {0}", __VA_ARGS__); std::raise(SIGINT); } }		
	#endif
#else
	#define OP_ENGINE_ASSERT(x, ...)
#endif

namespace OP
{
	class Logging
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return _engineLogger; }
		static std::shared_ptr<spdlog::logger>& GetAppLogger() { return _appLogger; }

	private:

		static std::shared_ptr<spdlog::logger> _engineLogger;
		static std::shared_ptr<spdlog::logger> _appLogger;

	};
}