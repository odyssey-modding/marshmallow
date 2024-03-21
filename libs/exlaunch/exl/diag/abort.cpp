/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "abort.hpp"

#include <algorithm>
#include <atomic>
#include <cstdarg>
#include <cstring>

namespace exl::diag {
    void NORETURN NOINLINE AssertionFailureImpl(const char* file, int line, const char* func,
                                                const char* expr, u64 value, const char* format,
                                                ...) {
        va_list args;
        va_start(args, format);
        exl::diag::AbortCtx ctx{
            .value = value,
            .line = line,
            .isAssertion = true,
            .file = file,
            .func = func,
            .expr = expr,
            .format = format,
            .args = args,
        };
        AbortImpl(ctx);
    }
    void NORETURN NOINLINE AssertionFailureImpl(const char* file, int line, const char* func,
                                                const char* expr, u64 value) {
        exl::diag::AbortCtx ctx{
            .value = value,
            .line = line,
            .isAssertion = true,
            .file = file,
            .func = func,
            .expr = expr,
        };
        AbortImpl(ctx);
    }
    void NORETURN NOINLINE AbortImpl(const char* file, int line, const char* func, const char* expr,
                                     u64 value, const char* format, ...) {
        va_list args;
        va_start(args, format);
        exl::diag::AbortCtx ctx{
            .value = value,
            .line = line,
            .isAssertion = false,
            .file = file,
            .func = func,
            .expr = expr,
            .format = format,
            .args = args,
        };
        AbortImpl(ctx);
    }
    void NORETURN NOINLINE AbortImpl(const char* file, int line, const char* func, const char* expr,
                                     u64 value) {
        exl::diag::AbortCtx ctx{
            .value = value,
            .line = line,
            .isAssertion = false,
            .file = file,
            .func = func,
            .expr = expr,
        };
        AbortImpl(ctx);
    }

};  // namespace exl::diag

/* C shim for libnx */
extern "C" NORETURN void exl_abort(Result r) {
    exl::diag::AbortCtx ctx{
        .value = r,
        .line = __LINE__,
        .isAssertion = false,
        .file = __FILE__,
        .func = __func__,
        .expr = "exl_abort",
    };
    exl::diag::AbortImpl(ctx);
}
