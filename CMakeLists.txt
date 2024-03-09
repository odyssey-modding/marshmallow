cmake_minimum_required(VERSION 3.21)
project(marshmallow CXX C ASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

add_compile_options(--target=aarch64-none-elf)
add_compile_options(-mcpu=cortex-a57+fp+simd+crypto+crc)
add_compile_options(-mno-implicit-float)
add_compile_options(-stdlib=libc++)
add_compile_options(-fPIC)
add_compile_options(-fstandalone-debug)
add_compile_options(-v -Wall -O3 -nostdlib -ffunction-sections -fdata-sections)
add_compile_options(-fno-rtti -fno-exceptions -fno-asynchronous-unwind-tables -fno-unwind-tables -fvisibility=default)
add_definitions(-D SWITCH)
add_link_options(-stdlib=libc++ -nostdlib)
add_link_options(-fPIC -shared -ffunction-sections -fdata-sections)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp -g ${ARCH}")

include(cmake/Helpers.cmake)

add_subdirectory(libs/exlaunch)
add_subdirectory(libs/nnsdk)
add_subdirectory(libs/marshmallow)

add_executable(subsdk9)
add_subdirectory(user)

target_link_libraries(subsdk9 PUBLIC marshmallow
        -T ${CMAKE_SOURCE_DIR}/cmake/linker.ld
        -fPIE
        -Wl,--whole-archive
        -Wl,--export-dynamic
        -Wl,--hash-style=sysv
        -Wl,-init,rtldInit
)
generate_npdm(subsdk9 ${CMAKE_SOURCE_DIR}/user/subsdk9.json)
make_nso_target(subsdk9)
set(FTP_TITLE_ID 0100000000010000 CACHE STRING "FTP Title ID")
deploy_nso_ftp(subsdk9 ${FTP_TITLE_ID})
