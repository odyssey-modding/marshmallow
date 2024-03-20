add_library(nnsdk INTERFACE)

target_compile_options(nnsdk INTERFACE -fno-strict-aliasing)
target_compile_options(nnsdk INTERFACE -fno-exceptions)
target_compile_options(nnsdk INTERFACE -Wall -Wextra)
target_compile_options(nnsdk INTERFACE -Wno-invalid-offsetof)

target_compile_definitions(nnsdk INTERFACE
        NN_SDK_MAJOR=${NN_SDK_MAJOR}
        NN_SDK_MINOR=${NN_SDK_MINOR}
        NN_SDK_PATCH=${NN_SDK_PATCH}
        NN_SDK_TYPE="${NN_SDK_TYPE}"
        NN_WARE_MAJOR=${NN_WARE_MAJOR}
        NN_WARE_MINOR=${NN_WARE_MINOR}
        NN_WARE_PATCH=${NN_WARE_PATCH}
)

target_include_directories(nnsdk
    SYSTEM
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/user/
        ${CMAKE_CURRENT_SOURCE_DIR}/submodule/include
)
