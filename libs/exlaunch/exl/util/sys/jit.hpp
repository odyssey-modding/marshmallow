#pragma once

#include "exl/common.hpp"
#include "exl/util/typed_storage.hpp"
#include "rw_pages.hpp"

#include <span>

#define JIT_CREATE(name, size)                          \
    namespace impl::name {                              \
        alignas(PAGE_SIZE)                              \
        __attribute__((section(".text." #name)))        \
        static const std::array<u8, size> s_Area {};    \
    }                                                   \
    auto name = exl::util::Jit(std::span(impl::name::s_Area));

namespace exl::util {

    class Jit {
        std::span<const u8> m_Rx;
        util::TypedStorage<RwPages> m_Pages;

        inline RwPages& GetPages() { return util::GetReference(m_Pages); }

    public:
        constexpr Jit(const std::span<const u8> rx) : m_Rx(rx) {}
        inline void Initialize() {
            util::ConstructAt(m_Pages, reinterpret_cast<uintptr_t>(m_Rx.data()), m_Rx.size());
        }

        inline void Flush() {
            GetPages().Flush();
        }

        inline uintptr_t GetRo() { return GetPages().GetRo(); }
        inline uintptr_t GetRw() { return GetPages().GetRw(); }
        inline uintptr_t GetSize() { return GetPages().GetSize(); }
    };
}
