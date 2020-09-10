#include "logger.h"

#if defined(__ANDROID__)
#	include <android/log.h>
#endif

namespace hwcpipe
{
void default_logger(LogSeverity severity, const char *message)
{
#if defined(__ANDROID__)
	android_LogPriority priority;

	switch (severity)
	{
		case LogSeverity::Verbose:
			priority = ANDROID_LOG_VERBOSE;
			break;
		case LogSeverity::Info:
			priority = ANDROID_LOG_INFO;
			break;
		case LogSeverity::Debug:
			priority = ANDROID_LOG_DEBUG;
			break;
		case LogSeverity::Warn:
			priority = ANDROID_LOG_WARN;
			break;
		case LogSeverity::Error:
			priority = ANDROID_LOG_ERROR;
			break;
		case LogSeverity::Fatal:
			priority = ANDROID_LOG_FATAL;
			break;
		default:
			priority = ANDROID_LOG_DEFAULT;
	}

	__android_log_write(priority, HWCPIPE_TAG, message);
#else
	const char *severity_str;

	switch (severity)
	{
		case LogSeverity::Verbose:
			severity_str = "Verbose";
			break;
		case LogSeverity::Info:
			severity_str = "Info";
			break;
		case LogSeverity::Debug:
			severity_str = "Debug";
			break;
		case LogSeverity::Warn:
			severity_str = "Warn";
			break;
		case LogSeverity::Error:
			severity_str = "Error";
			break;
		case LogSeverity::Fatal:
			severity_str = "Fatal";
			break;
		default:
			severity_str = "";
	}

	fmt::print("{} [{}]: {}\n", HWCPIPE_TAG, severity_str, message);
#endif
}

DebugLogCallback Logger::logger = &default_logger;

void Logger::set_logger(DebugLogCallback callback)
{
	logger = callback;
}
}        // namespace hwcpipe