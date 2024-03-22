#pragma once

#include <ArduinoJson.hpp>
#include <mallow/alloc.hpp>

// For reading from the config, look at Chapter 3.3, 3.4, 3.6, 3.7:
// https://files.arduinojson.org/v7/deserialization_tutorial.pdf
// For writing to the config, look at Chapter 4.2, 4.3, 4.6, 4.7
// https://files.arduinojson.org/v7/serialization_tutorial.pdf
namespace mallow::config {
    // externs are defined in user/src/mallowConfig.cpp
    extern const char* path;
    extern Allocator* getAllocator();

    // If loading fails once, it will not retry unless you pass true to this function.
    bool loadConfig(bool retry);
    bool saveConfig();
    bool isLoadedConfig();

    // If loading fails, it will return an empty JsonObject.
    ArduinoJson::JsonObject getConfig();
}  // namespace mallow::config
