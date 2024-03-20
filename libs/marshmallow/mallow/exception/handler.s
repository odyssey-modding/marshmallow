.section .text._ZN6mallow9exception14handlePhaseOneENS0_13ExceptionTypeEPNS0_19KernelExceptionInfoE, "ax", %progbits
.global _ZN6mallow9exception14handlePhaseOneENS0_13ExceptionTypeEPNS0_19KernelExceptionInfoE
.type _ZN6mallow9exception14handlePhaseOneENS0_13ExceptionTypeEPNS0_19KernelExceptionInfoE, %function
_ZN6mallow9exception14handlePhaseOneENS0_13ExceptionTypeEPNS0_19KernelExceptionInfoE:
    adrp x2, :got:_ZN6mallow9exception16extraGpRegistersE
    ldr x2, [x2, #:got_lo12:_ZN6mallow9exception16extraGpRegistersE]
    stp x9, x10, [x2]
    stp x11, x12, [x2, #16]
    stp x13, x14, [x2, #32]
    stp x15, x16, [x2, #48]
    stp x17, x18, [x2, #64]
    stp x19, x20, [x2, #80]
    stp x21, x22, [x2, #96]
    stp x23, x24, [x2, #112]
    stp x25, x26, [x2, #128]
    stp x27, x28, [x2, #144]
    str x29, [x2, #160]
    adrp x2, :got:_ZN6mallow9exception12fpuRegistersE
    ldr x2, [x2, #:got_lo12:_ZN6mallow9exception12fpuRegistersE]
    st4 {V0.16B, V1.16B, V2.16B, V3.16B}, [x2], #64
    st4 {V4.16B, V5.16B, V6.16B, V7.16B}, [x2], #64
    st4 {V8.16B, V9.16B, V10.16B, V11.16B}, [x2], #64
    st4 {V12.16B, V13.16B, V14.16B, V15.16B}, [x2], #64
    st4 {V16.16B, V17.16B, V18.16B, V19.16B}, [x2], #64
    st4 {V20.16B, V21.16B, V22.16B, V23.16B}, [x2], #64
    st4 {V24.16B, V25.16B, V26.16B, V27.16B}, [x2], #64
    st4 {V28.16B, V29.16B, V30.16B, V31.16B}, [x2], #64
    adrp x2, :got:_ZN6mallow9exception12handlerStackE
    ldr x2, [x2, :got_lo12:_ZN6mallow9exception12handlerStackE]
    add sp, x2, #0x20000
    b _ZN6mallow9exception14handlePhaseTwoENS0_13ExceptionTypeEPNS0_19KernelExceptionInfoE

.section .text._ZN6mallow9exception15handlePhaseFourEbPmPo, "ax", %progbits
.global _ZN6mallow9exception15handlePhaseFourEbPmPo
.type _ZN6mallow9exception15handlePhaseFourEbPmPo, %function
_ZN6mallow9exception15handlePhaseFourEbPmPo:
    ldp x11, x12, [x1, #16]
    ldp x13, x14, [x1, #32]
    ldp x15, x16, [x1, #48]
    ldp x17, x18, [x1, #64]
    ldp x19, x20, [x1, #80]
    ldp x21, x22, [x1, #96]
    ldp x23, x24, [x1, #112]
    ldp x25, x26, [x1, #128]
    ldp x27, x28, [x1, #144]
    ldr x29, [x1, #160]
    ld4 {V0.16B, V1.16B, V2.16B, V3.16B}, [x2], #64
    ld4 {V4.16B, V5.16B, V6.16B, V7.16B}, [x2], #64
    ld4 {V8.16B, V9.16B, V10.16B, V11.16B}, [x2], #64
    ld4 {V12.16B, V13.16B, V14.16B, V15.16B}, [x2], #64
    ld4 {V16.16B, V17.16B, V18.16B, V19.16B}, [x2], #64
    ld4 {V20.16B, V21.16B, V22.16B, V23.16B}, [x2], #64
    ld4 {V24.16B, V25.16B, V26.16B, V27.16B}, [x2], #64
    ld4 {V28.16B, V29.16B, V30.16B, V31.16B}, [x2], #64
    svc #0x28
