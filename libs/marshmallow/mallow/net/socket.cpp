#include <exl/lib.hpp>
#include <mallow/mallow.hpp>
#include <nn/nifm.h>
#include <nn/socket.hpp>

namespace mallow::net {
    // Can be redefined in user/src/mallowConfig.hpp.
    // If you change the sizes, you must also redefine the socket pool!
    // Don't forget to align the socket pool to the page size if you do.
    // Concur limit can be changed freely, but you probably don't need to change it
    // unless the game you're modding needs it.
    WEAK char socketPool[0x400000 + 0x20000] __attribute__((aligned(PAGE_SIZE)));
    WEAK u64 poolSize = 0x400000;
    WEAK u64 allocPoolSize = 0x20000;
    WEAK s32 concurLimit = 0xE;

    static bool isInitialized = false;
    nn::Result reasonForFailure = nn::ResultSuccess();

    nn::Result initializationResult() {
        return reasonForFailure;
    }

    void initializeNetwork() {
        if (isInitialized) return;

        reasonForFailure = nn::nifm::Initialize();
        if (reasonForFailure.IsFailure()) return;
        nn::nifm::SubmitNetworkRequestAndWait();

        reasonForFailure = nn::socket::Initialize(socketPool, poolSize, allocPoolSize, concurLimit);
        if (reasonForFailure.IsFailure()) return;
        // Prevent the socket service from being reinitialized
        exl::util::RwPages pages =
            exl::util::RwPages(reinterpret_cast<uintptr_t>(&nn::socket::Initialize), 4);
        pages.GetRw<u32>()[0] = exl::armv8::inst::Ret().Value();

        if (!nn::nifm::IsNetworkAvailable()) {
            dbg::debugPrint("Network not available");
            return;
        }

        isInitialized = true;
        reasonForFailure = nn::ResultSuccess();
    }
}  // namespace mallow::net
