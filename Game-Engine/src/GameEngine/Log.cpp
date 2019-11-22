#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace ge {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init() {
		// colour, timestamp %T, name of logger%n, log message%v%$
		spdlog::set_pattern("%^[%T] %n: %v%$");
		// mt - multithread
		s_CoreLogger = spdlog::stdout_color_mt("ENGINE");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}

}
