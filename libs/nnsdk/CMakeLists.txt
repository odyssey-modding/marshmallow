add_library(nnheaders INTERFACE)

target_compile_options(NintendoSDK PRIVATE -fno-strict-aliasing)
target_compile_options(NintendoSDK PRIVATE -fno-exceptions)
target_compile_options(NintendoSDK PRIVATE -Wall -Wextra)
target_compile_options(NintendoSDK PRIVATE -Wno-invalid-offsetof)

target_compile_definitions(NintendoSDK PRIVATE
        NN_SDK_MAJOR=${NN_SDK_MAJOR}
        NN_SDK_MINOR=${NN_SDK_MINOR}
        NN_SDK_PATCH=${NN_SDK_PATCH}
        NN_SDK_TYPE="${NN_SDK_TYPE}"
        NN_WARE_MAJOR=${NN_WARE_MAJOR}
        NN_WARE_MINOR=${NN_WARE_MINOR}
        NN_WARE_PATCH=${NN_WARE_PATCH}
)

target_include_directories(nnheaders
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/submodule/include
        ${CMAKE_CURRENT_SOURCE_DIR}/user/
)
