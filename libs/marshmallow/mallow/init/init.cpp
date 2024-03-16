#include <exl/hook/nx64/impl.hpp>
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
    exl::hook::nx64::Initialize();

    using namespace exl::util;
    if (mem_layout::s_SelfModuleIdx != mem_layout::s_RtldModuleIdx) {
        // we are not rtld!
        // todo: initialize exception handling
        // svcBreak(mem_layout::s_ModuleCount, mem_layout::s_SelfModuleIdx, mem_layout::s_RtldModuleIdx);

        // Stopgap solution for not triggering the sdk's user exception handler.
        // This will be replaced with our exception handler later.
        {
            exl::util::RwPages rtld(exl::util::GetRtldModuleInfo().m_Text.m_Start, 0x10);
            reinterpret_cast<u32*>(rtld.GetRw())[0] = 0x52800020; // mov w0, #1
            reinterpret_cast<u32*>(rtld.GetRw())[1] = 0xD4000501; // svc #0x28
        }

        // this allows atmosphere to detect symbols in our module!
        // this is only okay when we aren't rtld, as we would be overwriting the exception handler's route otherwise
        // see https://github.com/Atmosphere-NX/Atmosphere/blob/c8c76bf8f8f6f9587de5d8ac1bd7090635353e9b/stratosphere/creport/source/creport_modules.cpp#L297-L335
        // for how atmosphere detects the dynamic section
        {
            exl::util::RwPages startPage(exl::util::modules::GetSelfStart(), 0x4);
            reinterpret_cast<u32*>(startPage.GetRw())[0] = 0;
        }
    }
}
