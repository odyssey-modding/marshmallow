#include <array>
#include <exl/armv8.hpp>
#include <exl/armv8/register.hpp>
#include <exl/nx/arm/tls.h>
#include <exl/nx/kernel/svc.h>
#include <exl/types.h>
#include <exl/util/sys/mem_layout.hpp>
#include <exl/util/sys/rw_pages.hpp>
#include <mallow/exception/handler.hpp>
#include <mallow/logging/debug.hpp>
#include <mallow/logging/logger.hpp>
#include <nn/os.h>
#include <nn/types.h>

namespace mallow::exception {
    char handlerStack[0x20000] = {};

    HandlerFunc* getExceptionHandler() {
        return reinterpret_cast<HandlerFunc*>(reinterpret_cast<u8*>(armGetTls()) + 0x108);
    }

    HandlerFunc setExceptionHandler(HandlerFunc func) {
        auto* handler = getExceptionHandler();
        auto old = *handler;
        *handler = func;
        return old;
    }

    struct KernelExceptionInfo {
        std::array<nn::os::CpuRegister, 9> registers;
        nn::os::CpuRegister lr;
        nn::os::CpuRegister sp;
        nn::os::CpuRegister pc;
        u32 pstate;
        u32 afsr0;
        u32 afsr1;
        u32 esr;
        nn::os::CpuRegister far;
    };

    static_assert(sizeof(KernelExceptionInfo) == 0x78, "KernelExceptionInfo size mismatch");

    std::array<nn::os::CpuRegister, 21> extraGpRegisters;
    std::array<nn::os::FpuRegister, 32> fpuRegisters;
    KernelExceptionInfo* activeExceptionInfo = nullptr;


    extern void NORETURN handlePhaseFour(bool exiting, u64* extraGpRegisters,
                                u128* fpuRegisters);
    static void NORETURN handlePhaseThree(ExceptionInfo* info, bool exiting) {
        activeExceptionInfo->pc = info->pc;
        activeExceptionInfo->pstate = info->pstate;
        activeExceptionInfo->lr = info->lr;
        activeExceptionInfo->sp = info->sp;
        activeExceptionInfo->far = info->far;
        activeExceptionInfo->esr = info->esr;
        activeExceptionInfo->afsr0 = info->afsr0;
        activeExceptionInfo->afsr1 = info->afsr1;

        for (int i = 0; i < 9; i++) {
            activeExceptionInfo->registers[i] = info->gpRegisters[i];
        }

        for (int i = 0; i < 20; i++) {
            extraGpRegisters[i] = info->gpRegisters[i + 9];
        }

        extraGpRegisters[20] = info->fp;

        for (int i = 0; i < 32; i++) {
            fpuRegisters[i] = info->fpuRegisters[i];
        }

        activeExceptionInfo = nullptr;

        handlePhaseFour(exiting, &extraGpRegisters.data()->x, &fpuRegisters.data()->v);
    }
    extern void handlePhaseOne(ExceptionType type, KernelExceptionInfo* info);
    void NORETURN handlePhaseTwo(ExceptionType type, KernelExceptionInfo* info) {
        if (activeExceptionInfo != nullptr) {
            log::logLine("Second attempt at handling an exception, exiting!");
            svcReturnFromException(1);
        }
        activeExceptionInfo = info;
        std::array<nn::os::CpuRegister, 29> gpRegisters;
        ExceptionInfo userExceptionInfo = {
            .gpRegisters = gpRegisters,
            .fpuRegisters = fpuRegisters,
            .fp = extraGpRegisters[20],
            .lr = info->lr,
            .sp = info->sp,
            .pc = info->pc,
            .pstate = info->pstate,
            .afsr0 = info->afsr0,
            .afsr1 = info->afsr1,
            .esr = info->esr,
            .far = info->far,
            .type = type,
        };

        for (int i = 0; i < 9; i++) {
            userExceptionInfo.gpRegisters[i] = info->registers[i];
        }

        for (int i = 0; i < 20; i++) {
            userExceptionInfo.gpRegisters[i + 9] = extraGpRegisters[i];
        }

        auto handler = *getExceptionHandler();

        if (handler)
            handler(&userExceptionInfo);

        handlePhaseThree(&userExceptionInfo, true);
    }

    void ExceptionInfo::returnFromExecution(bool exiting) {
        handlePhaseThree(this, exiting);
    }

    void installHandlerAsSubsdk() {
        using namespace exl::armv8;
        struct RtldInit {
            inst::Instruction adrp_x2;
            inst::Instruction add_x2;
            u32 branch;
        };
        auto rtldStart = exl::util::GetRtldModuleInfo().m_Text.m_Start;
        exl::util::RwPages rtld(rtldStart, sizeof(RtldInit));
        auto relativeBranchAddress =
            reinterpret_cast<u8*>(&handlePhaseOne) - reinterpret_cast<u8*>(rtld.GetRo());
        rtld.GetRwRef<RtldInit>() = {
            .adrp_x2 = inst::Adrp(reg::X2, relativeBranchAddress),
            .add_x2 = inst::AddImmediate(reg::X2, reg::X2, relativeBranchAddress & 0xFFF),
            .branch = inst::BranchRegister(reg::X2).Value(),
        };
    }
}  // namespace mallow::exception
