#pragma once

#include <cstdio>
#include <exl/types.h>
#include <nn/fs.h>
#include <nn/os/os_Mutex.h>

namespace mallow::log::sink {
    class LogSink {
    protected:
        LogSink* next = nullptr;
        friend struct TraverserLogSink;
        friend void addLogSink(LogSink*);
        friend void removeLogSink(LogSink*);

    public:
        virtual ~LogSink() = default;
        virtual void log(const char* fmt, va_list args) = 0;
        virtual void logLine(const char* fmt, va_list args) = 0;
        virtual void logLine() = 0;
        virtual void logBufferHex(const char* buffer, std::size_t size) = 0;
    };

    LogSink& getLogSink();
    void addLogSink(LogSink* sink);
    void removeLogSink(LogSink* sink);

    struct DebugPrintSink : public LogSink {
        void log(const char* fmt, va_list args) override;
        void logLine(const char* fmt, va_list args) override;
        void logLine() override;
        void logBufferHex(const char* buffer, std::size_t size) override;
    };

    class NetworkSink : public LogSink {
        static bool initialized;
        s32 fileDescriptor = -1;
        nn::os::Mutex mutex;  // prevent concurrent writes to the socket

        void send(const char* buffer, std::size_t size);

    public:
        NetworkSink(const char* host, u16 port);

        // no copying or moving
        NetworkSink(const NetworkSink&) = delete;
        NetworkSink(NetworkSink&&) = delete;
        NetworkSink& operator=(const NetworkSink&) = delete;

        static NetworkSink fromConfig(const char* path);

        // If the connection is not established, the log functions will do nothing
        // Check the debug output for errors.
        bool isSuccessfullyConnected() const { return fileDescriptor >= 0; }

        void log(const char* fmt, va_list args) override;
        void logLine(const char* fmt, va_list args) override;
        void logLine() override;
        void logBufferHex(const char* buffer, std::size_t size) override;
    };

    class FileSink : public LogSink {
        nn::fs::FileHandle fileHandle;
        u64 offset = 0;
        bool isOpen = false;
        nn::os::Mutex mutex;  // prevent concurrent writes to the file

        void write(const char* buffer, std::size_t size);

    public:
        FileSink(const char* path);

        // no copying or moving
        FileSink(const FileSink&) = delete;
        FileSink(FileSink&&) = delete;
        FileSink& operator=(const FileSink&) = delete;

        void log(const char* fmt, va_list args) override;
        void logLine(const char* fmt, va_list args) override;
        void logLine() override;
        void logBufferHex(const char* buffer, std::size_t size) override;
    };

    // Utility function to log a buffer in hex format
    template <typename F>
    static void logBufferHelper(const char* buffer, std::size_t size, F&& log) {
        char row[16 * 3] = {};

        for (std::size_t i = 0; i < size; i++) {
            if (i % 16 == 0) {
                log(row, sizeof(row));
                row[0] = '\0';
            }
            snprintf(row + (i % 16) * 3, 4, "%02X ", buffer[i]);
        }

        if (row[0] != '\0')
            log(row, sizeof(row));
    }
}  // namespace mallow::log::sink
