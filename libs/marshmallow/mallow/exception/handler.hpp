#pragma once

#include <array>
#include <exl/types.h>
#include <nn/os.h>

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
