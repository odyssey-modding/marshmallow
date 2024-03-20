add_library(exlaunch STATIC)

target_sources(exlaunch
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/module.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/diag/abort.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/hook/nx64/hook_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/hook/nx64/inline_asm.s
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/hook/nx64/inline_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/nx/arm/cache.s
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/nx/kernel/svc.s
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/nx/kernel/virtmem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/reloc/elf.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/reloc/rtld/ld.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/reloc/rtld/ModuleObject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/reloc/rtld/utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/util/sys/cur_proc_handle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/util/sys/mem_layout.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/util/sys/rw_pages.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exl/util/random.cpp
)

target_include_directories(exlaunch
    SYSTEM
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
