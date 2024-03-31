#include <alloca.h>
#include <cstdio>
#include <exl/lib.hpp>
#include <mallow/mallow.hpp>
#include <nn/fs.h>
#include <nn/nifm.h>
#include <nn/socket.hpp>

namespace mallow::log::sink {
    struct TraverserLogSink : public LogSink {
        void log(const char* fmt, va_list args) override {
            auto* logSink = next;
            while (logSink) {
                logSink->log(fmt, args);
                logSink = logSink->next;
            }
        }

        void logLine(const char* fmt, va_list args) override {
            auto* logSink = next;
            while (logSink) {
                logSink->logLine(fmt, args);
                logSink = logSink->next;
            }
        }

        void logLine() override {
            auto* logSink = next;
            while (logSink) {
                logSink->logLine();
                logSink = logSink->next;
            }
        }

        void logBufferHex(const char* buffer, std::size_t size) override {
            auto* logSink = next;
            while (logSink) {
                logSink->logBufferHex(buffer, size);
                logSink = logSink->next;
            }
        }
    };

    static TraverserLogSink first;

    LogSink& getLogSink() {
        return first;
    }

    void addLogSink(LogSink* sink) {
        LogSink* current = &first;

        while (current->next)
            current = current->next;

        current->next = sink;
    }

    void removeLogSink(LogSink* sink) {
        LogSink* current = &first;
        while (current->next) {
            if (current->next == sink) {
                current->next = sink->next;
                sink->next = nullptr;
                return;
            }
            current = current->next;
        }
    }

    void DebugPrintSink::log(const char* fmt, va_list args) {
        char buffer[0x1000];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        svcOutputDebugString(buffer, sizeof(buffer));
    }
    void DebugPrintSink::logLine(const char* fmt, va_list args) {
        // OutputDebugString on emulator already adds newlines
        // don't really care about the few and far uart modders...
        log(fmt, args);
    }
    void DebugPrintSink::logLine() {
        svcOutputDebugString("", 0);
    }
    void DebugPrintSink::logBufferHex(const char* buffer, std::size_t size) {
        logBufferHelper(buffer, size,
                        [](const char* row, std::size_t size) { svcOutputDebugString(row, size); });
    }

    bool NetworkSink::initialized = false;

    NetworkSink::NetworkSink(const char* host, u16 port) : mutex(false) {
        if (!initialized) {
            mallow::net::initializeNetwork();
            
            if (mallow::net::initializationResult().IsFailure()) {
                dbg::debugPrint("NetworkSink: failed to initialize network");
                return;
            }

            if ((fileDescriptor = nn::socket::Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                dbg::debugPrint("NetworkSink: failed to create socket");
                return;
            }

            struct in_addr hostAddress = {};
            struct sockaddr_in serverAddress = {};
            if (!nn::socket::InetAton(host, &hostAddress)) {
                struct hostent* hostent = nn::socket::GetHostByName(host);
                if (hostent && hostent->h_addr_list[0]) {
                    hostAddress = *reinterpret_cast<struct in_addr*>(hostent->h_addr_list[0]);
                } else {
                    dbg::debugPrint("NetworkSink: failed to resolve host");
                    return;
                }
            }
            serverAddress.sin_addr = hostAddress;
            serverAddress.sin_port = nn::socket::InetHtons(port);
            serverAddress.sin_family = AF_INET;

            dbg::debugPrint("NetworkSink: connecting to %s:%d", host, port);

            s32 result = nn::socket::Connect(fileDescriptor,
                                             reinterpret_cast<struct sockaddr*>(&serverAddress),
                                             sizeof(serverAddress));

            if (result < 0) {
                dbg::debugPrint("NetworkSink: failed to connect to server");
                return;
            }

            bool tcpNoDelayOpt = true;
            nn::socket::SetSockOpt(fileDescriptor, 6, TCP_NODELAY, &tcpNoDelayOpt, sizeof(tcpNoDelayOpt));

            dbg::debugPrint("NetworkSink: connected to server");

            initialized = true;
        }
    }

    void NetworkSink::send(const char* buffer, std::size_t size) {
        if (fileDescriptor < 0)
            return;

        mutex.Lock();
        nn::socket::Send(fileDescriptor, buffer, size, 0);
        mutex.Unlock();
    }

    void NetworkSink::log(const char* fmt, va_list args) {
        auto size = 1 + vsnprintf(nullptr, 0, fmt, args);
        auto* buffer = reinterpret_cast<char*>(alloca(size));
        int len = vsnprintf(buffer, size, fmt, args);
        send(buffer, len);
    }

    void NetworkSink::logLine(const char* fmt, va_list args) {
        auto size = 2 + vsnprintf(nullptr, 0, fmt, args);
        auto* buffer = reinterpret_cast<char*>(alloca(size));
        int len = vsnprintf(buffer, size, fmt, args);
        buffer[len++] = '\n';
        send(buffer, len);
    }

    void NetworkSink::logLine() {
        send("\n", 1);
    }

    void NetworkSink::logBufferHex(const char* buffer, std::size_t size) {
        logBufferHelper(buffer, size, [this](const char* row, std::size_t size) {
            send(row, size);
            logLine();
        });
    }

    FileSink::FileSink(const char* path) : fileHandle(), mutex(false) {
        nn::fs::DeleteFile(path);

        if (nn::fs::CreateFile(path, 0).IsFailure()) {
            dbg::debugPrint("FileSink: failed to create file\n");
            return;
        }

        if (nn::fs::OpenFile(&fileHandle, path, nn::fs::OpenMode_Write | nn::fs::OpenMode_Append)
                .IsFailure()) {
            dbg::debugPrint("FileSink: failed to open file\n");
            return;
        }

        isOpen = true;
    }

    void FileSink::write(const char* buffer, std::size_t size) {
        if (!isOpen)
            return;

        mutex.Lock();
        nn::fs::WriteFile(fileHandle, offset, buffer, size,
                          nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
        offset += size;
        mutex.Unlock();
    }

    void FileSink::log(const char* fmt, va_list args) {
        auto size = 1 + vsnprintf(nullptr, 0, fmt, args);
        auto* buffer = reinterpret_cast<char*>(alloca(size));
        int len = vsnprintf(buffer, size, fmt, args);
        write(buffer, len);
    }

    void FileSink::logLine(const char* fmt, va_list args) {
        auto size = 2 + vsnprintf(nullptr, 0, fmt, args);
        auto* buffer = reinterpret_cast<char*>(alloca(size));
        int len = vsnprintf(buffer, size, fmt, args);
        buffer[len++] = '\n';
        write(buffer, len);
    }

    void FileSink::logLine() {
        write("\n", 1);
    }

    void FileSink::logBufferHex(const char* buffer, std::size_t size) {
        logBufferHelper(buffer, size, [this](const char* row, std::size_t size) {
            write(row, size);
            logLine();
        });
    }
}  // namespace mallow::log::sink
