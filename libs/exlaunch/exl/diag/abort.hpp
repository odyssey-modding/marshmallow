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

#pragma once

#include <cstdarg>
#include <optional>
#include "exl/common.hpp"

namespace exl::diag {

    struct AbortCtx {
        u64 value = 0;
        int line = 0;
        bool isAssertion = false;
        const char* file = "(unknown)";
        const char* func = "(unknown)";
        const char* expr = "(unknown)";
        const char* format = nullptr;
        va_list args;
    };

    void NORETURN NOINLINE AssertionFailureImpl(const char* file, int line, const char* func,
                                                const char* expr, u64 value, const char* format,
                                                ...) __attribute__((format(printf, 6, 7)));
    void NORETURN NOINLINE AssertionFailureImpl(const char* file, int line, const char* func,
                                                const char* expr, u64 value);

    void NORETURN NOINLINE AbortImpl(const char* file, int line, const char* func, const char* expr,
                                     u64 value, const char* format, ...)
        __attribute__((format(printf, 6, 7)));
    void NORETURN NOINLINE AbortImpl(const char* file, int line, const char* func, const char* expr,
                                     u64 value);

    void NORETURN NOINLINE AbortImpl(const AbortCtx&);
};  // namespace exl::diag
