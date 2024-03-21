#include <exl/hook/inline.hpp>
#include <exl/hook/replace.hpp>
#include <exl/hook/trampoline.hpp>

// If you don't want to use macros for defining hooks, you can do this instead.
/*
    #include <mallow/hook/helpers.hpp>

    using namespace mallow::hook;

    struct MyHook : public Trampoline<MyHook> {
        static void Callback() {
            Orig();
        }
    };
*/

namespace mallow::hook {
    template <typename T>
    using Inline = ::exl::hook::impl::InlineHook<T>;

    template <typename T>
    using Replace = ::exl::hook::impl::ReplaceHook<T>;

    template <typename T>
    using Trampoline = ::exl::hook::impl::TrampolineHook<T>;
}  // namespace mallow::hook
