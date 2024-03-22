#pragma once

#include <array>
#include <exl/types.h>
#include <nn/os.h>
/*
    Exception handling. If you don't know what you're doing, don't bother trying to use this for now.

    mallow::exception::setExceptionHandler([](mallow::exception::ExceptionInfo* info) {
        // Basic example exception handler.
        mallow::log::logLine("Exception type: %u", static_cast<u32>(info->type));
        mallow::log::logLine("Exception PC: 0x%016llX", info->pc.x);
        mallow::log::logLine("Exception SP: 0x%016llX", info->sp.x);
        mallow::log::logLine("Exception LR: 0x%016llX", info->lr.x);
        mallow::log::logLine("Exception FP: 0x%016llX", info->fp.x);
        // This is the contents of SPSR_EL1 & 0xF0000000
        mallow::log::logLine("Exception PSTATE: 0x%08X", info->pstate);
        // https://esr.arm64.dev/ for ESR decoding
        mallow::log::logLine("Exception ESR: 0x%08X", info->esr);
        mallow::log::logLine("Exception AFSR0: 0x%08X", info->afsr0);
        mallow::log::logLine("Exception AFSR1: 0x%08X", info->afsr1);
        mallow::log::logLine("Exception FAR: 0x%016llX", info->far.x);

        info->exit();
    });
*/

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

    // 64-bit Exception Info
    struct ExceptionInfo {
        std::array<nn::os::CpuRegister, 29>& gpRegisters;
        std::array<nn::os::FpuRegister, 32>& fpuRegisters;
        nn::os::CpuRegister fp;
        nn::os::CpuRegister lr;
        nn::os::CpuRegister sp;
        nn::os::CpuRegister pc;
        u32 pstate;
        u32 afsr0;
        u32 afsr1;
        u32 esr;
        nn::os::CpuRegister far;
        ExceptionType type;

        // Only use this if you know what you're doing
        void NORETURN ignoreException() {
            pc.x += 4;
            returnFromExecution(false);
        }

        // Only use this if you know what you're doing
        void NORETURN returnFromExecution(bool exiting);

        // If you're done with exception handling, use this to exit the process.
        void NORETURN exit() { returnFromExecution(true); }
    };

    using HandlerFunc = void (*)(ExceptionInfo* info);
    HandlerFunc setExceptionHandler(HandlerFunc handler);

    void initialize();
    void installHandlerAsSubsdk();

    class ScopedHandler {
        void (*previousHandler)(ExceptionInfo* info);

    public:
        ScopedHandler(void (*handler)(ExceptionInfo* info)) {
            previousHandler = setExceptionHandler(handler);
        }
        ~ScopedHandler() { setExceptionHandler(previousHandler); }
    };

    static ALWAYS_INLINE void causeException() {
        asm volatile("svc #0xBEEF");
    }
}  // namespace mallow::exception
