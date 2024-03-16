add_library(marshmallow STATIC)

target_sources(marshmallow
    PRIVATE
        mallow/init/init.cpp
        mallow/init/crt0.s
        mallow/exception/handler.cpp
        mallow/exception/handler.hpp
        mallow/debug.hpp
)

target_include_directories(marshmallow
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(marshmallow PUBLIC exlaunch nnsdk)
