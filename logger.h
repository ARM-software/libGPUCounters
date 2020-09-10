#pragma once

#include <fmt/format.h>

#define HWCPIPE_TAG "HWCPipe"

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

class Logger
{
  public:
	virtual ~Logger() = default;

	static void set_logger(DebugLogCallback callback);

	template <typename... Args>
	void log(LogSeverity severity, const std::string &format, Args... args) const
	{
		logger(severity, fmt::format(format, args...).c_str());
	}

  private:
	static DebugLogCallback logger;
};
}        // namespace hwcpipe
