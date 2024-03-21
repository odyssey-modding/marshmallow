add_library(marshmallow STATIC)

target_sources(marshmallow
    PRIVATE
        mallow/alloc.hpp
        mallow/exception/abort.cpp
        mallow/exception/handler.cpp
        mallow/exception/handler.hpp
        mallow/exception/handler.s
        mallow/init/init.cpp
        mallow/init/initArgs.hpp
        mallow/init/crt0.s
        mallow/logging/logger.cpp
        mallow/logging/logger.hpp
        mallow/logging/logSinks.cpp
        mallow/logging/logSinks.hpp
        mallow/logging/debug.hpp
)

target_include_directories(marshmallow
    SYSTEM
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(marshmallow PUBLIC exlaunch nnsdk)
