#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <exl/nx/kernel/svc.h>
#include <exl/reloc/rtld.hpp>
#include <exl/util/sys/mem_layout.hpp>
#include <mallow/logging/logger.hpp>
#include <nn/diag.h>

namespace mallow::dbg {
    static void debugPrint(const char* fmt, ...) {
        char buffer[2048] = {};
        va_list args;

        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        svcOutputDebugString(buffer, sizeof(buffer));
    }

    static const char* getModuleName(int moduleIndex) {
        struct ModuleName {
            u32 unknown;
            u32 name_length;
            const char name[];
        };

        auto* moduleName =
            reinterpret_cast<ModuleName*>(exl::util::GetModuleInfo(moduleIndex).m_Rodata.m_Start);
        if (moduleName->name_length == 0) {
            return nullptr;
        }
        const char* name = moduleName->name;
        const char* result;
        while ((result = std::strchr(name, '\\'))) {
            name = ++result;
        }
        return name;
    }

    static int getModuleIndex(uintptr_t addr) {
        for (int i = 0; i < exl::util::mem_layout::s_ModuleCount; i++) {
            auto& module = exl::util::GetModuleInfo(i);
            if (addr >= module.m_Total.m_Start && addr < module.m_Total.GetEnd()) {
                return i;
            }
        }
        return -1;
    }

    static void walkStack() {
        struct StackFrame {
            uintptr_t fp;
            uintptr_t lr;
        };
        // incredibly useful builtin for getting the current stack frame
        auto* stackFrame = reinterpret_cast<StackFrame*>(__builtin_frame_address(0));

        while (stackFrame) {
            auto sp = reinterpret_cast<uintptr_t>(stackFrame);
            if (sp < exl::util::mem_layout::s_Aslr.m_Start ||
                sp > exl::util::mem_layout::s_Aslr.GetEnd()) {
                // we probably aren't in the stack anymore
                break;
            }

            auto moduleIndex = getModuleIndex(stackFrame->lr);

            if (moduleIndex >= 0) {
                auto* moduleName = getModuleName(moduleIndex);
                if (moduleName) {
                    log::logLine("Frame location: %s+0x%zx %p", moduleName,
                                 stackFrame->lr -
                                     exl::util::GetModuleInfo(moduleIndex).m_Total.m_Start,
                                 stackFrame->lr);
                } else {
                    dbg::debugPrint("Frame location: %p", stackFrame->lr);
                }
            } else {
                dbg::debugPrint("Frame location: %p", stackFrame->lr);
            }

            stackFrame = reinterpret_cast<StackFrame*>(stackFrame->fp);
        }
    }
}  // namespace mallow::dbg
