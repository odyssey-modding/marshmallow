#pragma once

#include <cstdarg>
#include <mallow/logging/logSinks.hpp>

namespace mallow::log {
    void log(const char* fmt, ...);
    void log(const char* fmt, va_list args);
    void logLine(const char* fmt, ...);
    void logLine(const char* fmt, va_list args);
    void logLine();

    void logBufferHex(const char* buffer, std::size_t size);

    enum class LoggerSinks {
        DebugPrint,
        Network,
        File,
    };
}  // namespace mallow::log
