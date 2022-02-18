#pragma once

#include <Opium/Core.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

// Log macros for Engine
#define OP_ENGINE_FATAL(...) ::Opium::Logging::GetEngineLogger()->critical(__VA_ARGS__);
#define OP_ENGINE_ERROR(...) ::Opium::Logging::GetEngineLogger()->error(__VA_ARGS__);
#define OP_ENGINE_WARN(...) ::Opium::Logging::GetEngineLogger()->warn(__VA_ARGS__);
#define OP_ENGINE_INFO(...) ::Opium::Logging::GetEngineLogger()->info(__VA_ARGS__);
#define OP_ENGINE_TRACE(...) ::Opium::Logging::GetEngineLogger()->trace(__VA_ARGS__);

// Log macros for app
#define OP_APP_FATAL(...) ::Opium::Logging::GetAppLogger()->critical(__VA_ARGS__);
#define OP_APP_ERROR(...) ::Opium::Logging::GetAppLogger()->error(__VA_ARGS__);
#define OP_APP_WARN(...) ::Opium::Logging::GetAppLogger()->warn(__VA_ARGS__);
#define OP_APP_INFO(...) ::Opium::Logging::GetAppLogger()->info(__VA_ARGS__);
#define OP_APP_TRACE(...) ::Opium::Logging::GetAppLogger()->trace(__VA_ARGS__);

namespace Opium
{
	class OPIUM_API Logging
	{
	public:
		static void Init();


		inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return _engineLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetAppLogger() { return _appLogger; }

	private:
		static std::shared_ptr<spdlog::logger> _engineLogger;
		static std::shared_ptr<spdlog::logger> _appLogger;

	};
}