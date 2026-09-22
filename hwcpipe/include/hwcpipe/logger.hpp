/*
 * Copyright (c) 2026 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <string_view>
#include <array>

namespace hwcpipe {
namespace {
    const std::array<std::string_view, 3> log_level_string{
        "INFO", "WARN", "ERROR"
    };

    enum class log_level : uint8_t {
        info,
        warning,
        error
    };
}

namespace details {
/**
 * Default stream used is stderr.
 */
inline FILE *&stream() {
    static FILE *s = stderr;
    return s;
}

inline std::mutex &lock() {
    static std::mutex m;
    return m;
}

/**
 * Set stream.
 * @param f - FILE stream
 */
inline void set_stream(FILE *f) {
    std::lock_guard<std::mutex> g(lock());
    stream() = (f ? f : stderr);
}

/**
 * Set file path for log file.
 * @param path - log file path
 */
inline bool set_file(const char *path) {
    if (!path)
        return false;
    FILE *f = std::fopen(path, "a");
    if (!f)
        return false;
    set_stream(f);
    return true;
}

inline void vlog(const log_level level, const char *fmt, va_list args) {
    std::lock_guard<std::mutex> g(lock());
    FILE *out = stream();
    if (!out)
        return;

    std::fprintf(out, "[%s]: ", log_level_string[static_cast<int>(level)].data());
    std::vfprintf(out, fmt, args);
    std::fprintf(out, "\n");
    std::fflush(out);
}

} // namespace details

#define LOG_INFO ::hwcpipe::log_level::info
#define LOG_WARN ::hwcpipe::log_level::warning
#define LOG_ERROR ::hwcpipe::log_level::error

inline void log_item(const log_level level, const char *fmt, ...) {
    if (!fmt)
        return;
    va_list args;
    va_start(args, fmt);
    details::vlog(level, fmt, args);
    va_end(args);
}

} // namespace hwcpipe