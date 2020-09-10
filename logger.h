#pragma once

#include <fmt/format.h>

#define HWCPIPE_TAG "HWCPipe"

#if defined(__ANDROID__)
#	include <android/log.h>
#endif

namespace hwcpipe
{
enum class LogSeverity
{
	Verbose,
	Info,
	Debug,
	Warn,
	Error,
	Fatal
};

typedef void (*DebugLogCallback)(LogSeverity severity, const char *message);

#if defined(__ANDROID__)
void default_logger(LogSeverity severity, const char *message)
{
	android_LogPriority priority;

	switch (severity)
	{
		case LogSeverity::Verbose:
			priority = ANDROID_LOG_VERBOSE;
		case LogSeverity::Info:
			priority = ANDROID_LOG_INFO;
		case LogSeverity::Debug:
			priority = ANDROID_LOG_DEBUG;
		case LogSeverity::Warn:
			priority = ANDROID_LOG_WARN;
		case LogSeverity::Error:
			priority = ANDROID_LOG_ERROR;
		case LogSeverity::Fatal:
			priority = ANDROID_LOG_FATAL;
		default:
			priority = ANDROID_LOG_DEFAULT;
	}

	__android_log_print(priority, HWCPIPE_TAG, __VA_ARGS__);
}
#else
void default_logger(LogSeverity severity, const char *message)
{
	const char *severity_str;

	switch (severity)
	{
		case LogSeverity::Verbose:
			severity_str = "Verbose";
		case LogSeverity::Info:
			severity_str = "Info";
		case LogSeverity::Debug:
			severity_str = "Debug";
		case LogSeverity::Warn:
			severity_str = "Warn";
		case LogSeverity::Error:
			severity_str = "Error";
		default:
			severity_str = "";
	}

	fmt::print("{} [{}]: {}\n", HWCPIPE_TAG, severity_str, message);
}
#endif

class Logger
{
  public:
	virtual ~Logger() = default;

	void set_logger(DebugLogCallback callback)
	{
		logger = callback;
	}

	DebugLogCallback get_logger() const
	{
		return logger;
	}

	template <typename... Args>
	void log(LogSeverity severity, const std::string &format, Args... args) const
	{
		logger(severity, fmt::format(format, args...).c_str());
	}

  private:
	DebugLogCallback logger = &default_logger;
};
}        // namespace hwcpipe
