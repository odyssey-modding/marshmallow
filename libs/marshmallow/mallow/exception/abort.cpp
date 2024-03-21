#include <exl/diag/abort.hpp>
#include <exl/nx/kernel/svc.h>
#include <mallow/exception/handler.hpp>
#include <mallow/logging/logger.hpp>

namespace exl::diag {
    void WEAK NORETURN NOINLINE AbortImpl(const AbortCtx & ctx) {
        if (ctx.isAssertion) 
            mallow::log::log("Assertion failed: ");
        else
            mallow::log::log("Abort: ");
        if (ctx.format) {
            mallow::log::logLine(ctx.format, ctx.args);
        }

        mallow::log::logLine("%s == %llu", ctx.expr, ctx.value);
        mallow::log::logLine("  at %s:%d", ctx.file, ctx.line);

        svcBreak(ctx.isAssertion ? BreakReason_Assert : BreakReason_Panic, ctx.value, 0);

        UNREACHABLE;
    }
}
