#pragma once

#include <exl/types.h>

namespace mallow::exception {
    enum class ExceptionType : u32 {
        Init = 0x000,
        InstructionAbort = 0x100,
        DataAbort = 0x101,
        UnalignedInstruction = 0x102,
        UnalignedData = 0x103,
        UndefinedInstruction = 0x104,
        ExceptionInstruction = 0x105,
        MemorySystemError = 0x106,
        FpuException = 0x200,
        InvalidSystemCall = 0x301,
        SystemCallBreak = 0x302,

        AtmosphereStdAbort = 0xFFE,
    };

    class ExceptionInfo {
        uintptr_t r[29];
        uintptr_t fp;
        uintptr_t lr;
        uintptr_t sp;
        uintptr_t pc;
        u32 pstate;
        u32 afsr0;
        u32 afsr1;
        u32 esr;
        uintptr_t far;
        ExceptionType type;

    public:
        // work in progress
    };
}
