#include <ArduinoJson.hpp>
#include <cstring>
#include <exl/types.h>
#include <mallow/alloc.hpp>
#include <mallow/config.hpp>
#include <mallow/logging/logger.hpp>
#include <nn/fs.h>
#include <tuple>
#include <vapours/results/fs_results.hpp>

namespace mallow::config {
    class JsonAllocator : public ArduinoJson::Allocator {
    public:
        void* allocate(size_t size) override { return getAllocator()->allocate(size); }
        void deallocate(void* ptr) override { getAllocator()->free(ptr); }
        void* reallocate(void* ptr, size_t new_size) override {
            return getAllocator()->reallocate(ptr, new_size);
        }
    };

    static JsonAllocator allocator = {};
    static std::tuple<ArduinoJson::JsonDocument, u8*, bool> config =
        std::make_tuple(ArduinoJson::JsonDocument(&allocator), nullptr, false);
    static bool configFailed = false;

    bool isLoadedConfig() {
        return std::get<2>(config);
    }

    ArduinoJson::JsonObject getConfig() {
        if (!isLoadedConfig()) {
            std::get<0>(config).to<ArduinoJson::JsonObject>();
        }

        return std::get<0>(config).as<ArduinoJson::JsonObject>();
    }
    bool loadConfig(bool retry) {
        if (!std::get<0>(config).is<ArduinoJson::JsonObject>())
            std::get<0>(config).to<ArduinoJson::JsonObject>();

        if (configFailed && !retry)
            return false;

        nn::fs::FileHandle file;
        auto res = nn::fs::OpenFile(&file, path, nn::fs::OpenMode_Read);
        if (nn::fs::ResultPathNotFound::Includes(res)) {
            nn::fs::CreateFile(path, 0);

            if (nn::fs::OpenFile(&file, path, nn::fs::OpenMode_Read).IsFailure()) {
                log::logLine("Failed to open config file");
                configFailed = true;
                return false;
            }
        } else if (res.IsFailure()) {
            log::logLine("Failed to open config file");
            configFailed = true;
            return false;
        }

        s64 fileSize;
        if (nn::fs::GetFileSize(&fileSize, file).IsFailure()) {
            log::logLine("Failed to get file size");
            configFailed = true;
            return false;
        }

        auto* allocator = getAllocator();
        auto data = allocator->allocate<u8>(fileSize + 1);
        res = nn::fs::ReadFile(file, 0, data, fileSize);
        nn::fs::CloseFile(file);
        if (res.IsFailure()) {
            log::logLine("Failed to read config file");
            configFailed = true;
            return false;
        }

        data[fileSize] = '\0';
        auto& document = std::get<0>(config);
        auto error = ArduinoJson::deserializeJson(document, data);

        if (error) {
            log::logLine("Failed to parse config file");
            configFailed = true;
            return false;
        }

        if (std::get<1>(config))
            allocator->free(std::get<1>(config));

        std::get<1>(config) = data;
        std::get<2>(config) = true;
        configFailed = false;

        log::logLine("Config loaded");

        return true;
    }
    bool saveConfig() {
        nn::fs::FileHandle file;
        auto res = nn::fs::OpenFile(&file, path, nn::fs::OpenMode_Write);
        if (nn::fs::ResultPathNotFound::Includes(res)) {
            nn::fs::CreateFile(path, 0);

            if (nn::fs::OpenFile(&file, path, nn::fs::OpenMode_Write).IsFailure()) {
                log::logLine("Failed to open config file");
                return false;
            }
        } else if (res.IsFailure()) {
            log::logLine("Failed to open config file");
            return false;
        }

        auto& document = std::get<0>(config);
        auto size = ArduinoJson::measureJsonPretty(document);

        res = nn::fs::SetFileSize(file, size);
        if (res.IsFailure()) {
            log::logLine("Failed to set file size");
            return false;
        }

        auto* outData = getAllocator()->allocate<u8>(size + 1);
        std::memset(outData, 0, size + 1);
        ArduinoJson::serializeJsonPretty(document, outData, size);

        res = nn::fs::WriteFile(file, 0, outData, size,
                                nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));

        getAllocator()->free(outData);
        nn::fs::CloseFile(file);

        return res.IsFailure();
    }
}  // namespace mallow::config
