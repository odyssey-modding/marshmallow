#include <mallow/mallow.hpp>

namespace mallow::config {
    const char* path = "sd:/mallow.json";
    Allocator* getAllocator() {
        static DefaultAllocator allocator = {};
        return &allocator;
    }
}
