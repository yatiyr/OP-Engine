#include <Precomp.h>
#include <Op/Logging.h>

namespace OP
{
	std::shared_ptr<spdlog::logger> Logging::_engineLogger;
	std::shared_ptr<spdlog::logger> Logging::_appLogger;

	void Logging::Init()
	{

		spdlog::set_pattern("%^[%T] %n(ThreadID %t): %v%$");
		
		_engineLogger = spdlog::stdout_color_mt("OP", spdlog::color_mode::always);
		

		_engineLogger->set_level(spdlog::level::trace);

		_appLogger = spdlog::stdout_color_mt("APP");
		_appLogger->set_level(spdlog::level::trace);
	}
}