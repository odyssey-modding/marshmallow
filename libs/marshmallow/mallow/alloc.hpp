#include <cstddef>
#include <cstdint>
#include <cstdlib>

extern "C" void* malloc(size_t size);
extern "C" void* aligned_alloc(size_t alignment, size_t size);
extern "C" void* realloc(void* ptr, size_t size);
extern "C" void free(void* ptr);

namespace mallow {
    class Allocator {
    public:
        virtual ~Allocator() = default;
        virtual void* allocate(size_t size, size_t alignment = sizeof(uintptr_t)) = 0;
        virtual void* reallocate(void* ptr, size_t size) = 0;
        virtual void free(void* ptr) = 0;
    };

    class DefaultAllocator : public Allocator {
    public:
        void* allocate(size_t size, size_t alignment) override {
            return std::aligned_alloc(alignment, size);
        }

        void* reallocate(void* ptr, size_t size) override { return std::realloc(ptr, size); }

        void free(void* ptr) override { std::free(ptr); }
    };
}  // namespace mallow
