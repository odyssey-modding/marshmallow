#include <mallow/logging/logger.hpp>

namespace mallow::log {
    void log(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log(fmt, args);
        va_end(args);
    }

    void log(const char* fmt, va_list args) {
        sink::getLogSink().log(fmt, args);
    }

    void logLine(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        logLine(fmt, args);
        va_end(args);
    }

    void logLine(const char* fmt, va_list args) {
        sink::getLogSink().logLine(fmt, args);
    }

    void logLine() {
        sink::getLogSink().logLine();
    }

    void logBufferHex(const char* buffer, std::size_t size) {
        sink::getLogSink().logBufferHex(buffer, size);
    }
}  // namespace mallow::log
