#include <exl/util/sys/mem_layout.hpp>
#include <exl/util/modules.hpp>
#include <exl/patch/patcher_impl.hpp>
#include <exl/nx/kernel/virtmem.h>
#include <exl/util/sys/cur_proc_handle.hpp>

extern "C" {
    /* These magic symbols are provided by the linker.  */
    extern void (*__preinit_array_start[])(void) __attribute__((weak));

    extern void (*__preinit_array_end[])(void) __attribute__((weak));

    extern void (*__init_array_start[])(void) __attribute__((weak));

    extern void (*__init_array_end[])(void) __attribute__((weak));

    /* Exported by program. */
    void userMain();

    /* Optionally exported by program. */
    __attribute__((weak)) extern void userInit();

    void __init_array(void) {
        size_t count;
        size_t i;

        count = __preinit_array_end - __preinit_array_start;
        for (i = 0; i < count; i++)
            __preinit_array_start[i]();

        count = __init_array_end - __init_array_start;
        for (i = 0; i < count; i++)
            __init_array_start[i]();
    }

    static void *initArgs[2] = {};

    void getInitArgs(void **x0, void **x1) {
        if (x0)
            *x0 = initArgs[0];
        if (x1)
            *x1 = initArgs[1];
    }

    void entrypointInit(void *x0, void *x1) {
        userInit();
        __init_array();
        initArgs[0] = x0;
        initArgs[1] = x1;
        userMain();
    }

    void rtldInit() {
        entrypointInit(nullptr, nullptr);
    }
}

extern "C" void userInit() {
    exl::util::impl::InitMemLayout();
    virtmemSetup();
    exl::patch::impl::InitPatcherImpl();
    exl::util::proc_handle::Get();

    using namespace exl::util;
    if (mem_layout::s_SelfModuleIdx != mem_layout::s_RtldModuleIdx) {
        // we are not rtld!
        // todo: initialize exception handling

        // Stopgap solution for not triggering the sdk's user exception handler.
        // This will be replaced with our exception handler
        exl::util::RwPages rtld(exl::util::GetRtldModuleInfo().m_Text.m_Start, 0x10);
        reinterpret_cast<u32*>(rtld.GetRw())[0] = 0x52800020;
        reinterpret_cast<u32*>(rtld.GetRw())[1] = 0xD4000501;
    }
}
