#pragma once


#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace TruthEngine::Core {

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
#define TE_LOG_CORE_TRACE(...)  ::TruthEngine::Core::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TE_LOG_CORE_INFO(...)  ::TruthEngine::Core::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TE_LOG_CORE_WARN(...)  ::TruthEngine::Core::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TE_LOG_CORE_ERROR(...)  ::TruthEngine::Core::Log::GetCoreLogger()->error(__VA_ARGS__)

#define TE_LOG_APP_TRACE(...)  ::TruthEngine::Core::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TE_LOG_APP_INFO(...)  ::TruthEngine::Core::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TE_LOG_APP_WARN(...)  ::TruthEngine::Core::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TE_LOG_APP_ERROR(...)  ::TruthEngine::Core::Log::GetCoreLogger()->error(__VA_ARGS__)