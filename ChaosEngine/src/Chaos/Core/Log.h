#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Chaos
{
	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> sCoreLogger;
		static std::shared_ptr<spdlog::logger> sClientLogger;

	public:
		static void Init();
		

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return sClientLogger; }
	};
	
	//CORE MACROS
#define LOGCORE_TRACE(...)	::Chaos::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOGCORE_INFO(...)	::Chaos::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOGCORE_WARN(...)	::Chaos::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOGCORE_ERROR(...)	::Chaos::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOGCORE_FATAL(...)	::Chaos::Log::GetCoreLogger()->fatal(__VA_ARGS__)


	//CLIENT MACROS
#define LOGTRACE(...)	::Chaos::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOGINFO(...)	::Chaos::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOGWARN(...)	::Chaos::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOGERROR(...)	::Chaos::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOGFATAL(...)	::Chaos::Log::GetClientLogger()->fatal(__VA_ARGS__)
}