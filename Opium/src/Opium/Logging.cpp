#include <Precomp.h>
#include <Opium/Logging.h>

namespace Opium
{
	std::shared_ptr<spdlog::logger> Logging::_engineLogger;
	std::shared_ptr<spdlog::logger> Logging::_appLogger;

	void Logging::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		_engineLogger = spdlog::stdout_color_mt("OPIUM");
		_engineLogger->set_level(spdlog::level::trace);

		_appLogger = spdlog::stdout_color_mt("APP");
		_appLogger->set_level(spdlog::level::trace);
	}
}