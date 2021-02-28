#pragma once


#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace TruthEngine {

	class Log {

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_ClientLogger;

	};

}

//Core Log macros
#define TE_LOG_CORE_TRACE(...)  ::TruthEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TE_LOG_CORE_INFO(...)  ::TruthEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TE_LOG_CORE_WARN(...)  ::TruthEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TE_LOG_CORE_ERROR(...)  ::TruthEngine::Log::GetCoreLogger()->error(__VA_ARGS__)

#define TE_LOG_APP_TRACE(...)  ::TruthEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TE_LOG_APP_INFO(...)  ::TruthEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TE_LOG_APP_WARN(...)  ::TruthEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TE_LOG_APP_ERROR(...)  ::TruthEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
