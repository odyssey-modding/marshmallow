// This file is the entry point for user code.
#include <mallow/mallow.hpp>

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
    auto config = mallow::config::getConfig();
    if (config["logger"]["ip"].is<const char*>()) {
        static NetworkSink networkSink = NetworkSink(
            config["logger"]["ip"],
            config["logger"]["port"] | 3080
        );
        if (networkSink.isSuccessfullyConnected())
            addLogSink(&networkSink);
        else
            mallow::log::logLine("Failed to connect to the network sink");
    } else {
        mallow::log::logLine("The network logger was configured improperly.");
    }
}

extern "C" void userMain() {
    nn::fs::MountSdCardForDebug("sd");

    setupLogging();
}
