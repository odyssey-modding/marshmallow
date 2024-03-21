// This file is the entry point for user code.
#include <mallow/exception/handler.hpp>
#include <mallow/logging/logger.hpp>

// If you don't know what to do, see exlaunch for an example.
// https://github.com/shadowninja108/exlaunch/blob/main/source/program/main.cpp#L3-L37

// Prepares all of the log sinks. If you want to create your own, check
// mallow/logging/logSinks.hpp and mallow/logging/logSinks.cpp
static void setupLogging() {
    using namespace mallow::log::sink;
    // This sink writes to a file on the SD card.
    static FileSink fileSink = FileSink("sd:/mallow.log");
    addLogSink(&fileSink);

    // This sink writes to a network socket on a host computer. Raw logs are sent with no
    // processing.
    static NetworkSink networkSink = NetworkSink("192.168.1.245", 3080);
    if (networkSink.isSuccessfullyConnected())
        addLogSink(&networkSink);
    else
        mallow::log::logLine("Failed to connect to the network sink");
}

extern "C" void userMain() {
    nn::fs::MountSdCardForDebug("sd");
    setupLogging();
    mallow::log::logLine("Hello, world!");

    mallow::exception::setExceptionHandler([](mallow::exception::ExceptionInfo* info) {
        // Basic example exception handler.
        mallow::log::logLine("Exception type: %u", static_cast<u32>(info->type));
        mallow::log::logLine("Exception PC: 0x%016llX", info->pc.x);
        mallow::log::logLine("Exception SP: 0x%016llX", info->sp.x);
        mallow::log::logLine("Exception LR: 0x%016llX", info->lr.x);
        mallow::log::logLine("Exception FP: 0x%016llX", info->fp.x);
        // This is the contents of SPSR_EL1 & 0xF0000000
        mallow::log::logLine("Exception PSTATE: 0x%08X", info->pstate);
        // https://esr.arm64.dev/ for ESR decoding
        mallow::log::logLine("Exception ESR: 0x%08X", info->esr);
        mallow::log::logLine("Exception AFSR0: 0x%08X", info->afsr0);
        mallow::log::logLine("Exception AFSR1: 0x%08X", info->afsr1);
        mallow::log::logLine("Exception FAR: 0x%016llX", info->far.x);

        info->exit();
    });
}
