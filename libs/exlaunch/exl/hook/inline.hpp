#pragma once

#include <exl/common.hpp>
#include <exl/diag/assert.hpp>
#include <nn/ro.h>

#include "base.hpp"

#define HOOK_DEFINE_INLINE(name)                        \
struct name : public ::exl::hook::impl::InlineHook<name>

namespace exl::hook::impl {

    template<typename Derived>
    struct InlineHook {
        
        template<typename T = Derived>
        using CallbackFuncPtr = decltype(&T::Callback);

        static ALWAYS_INLINE void InstallAtOffset(ptrdiff_t address) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            hook::HookInline(util::modules::GetTargetStart() + address, Derived::Callback);
        }

        static ALWAYS_INLINE void InstallAtPtr(uintptr_t ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();
            
            hook::HookInline(ptr, Derived::Callback);
        }

        static ALWAYS_INLINE void InstallAtSymbol(const char* symbol) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            uintptr_t address = 0;
            EXL_ASSERT(nn::ro::LookupSymbol(&address, symbol).IsSuccess(), "Symbol not found!");
            
            hook::HookInline(address, Derived::Callback);
        }
    };
}
