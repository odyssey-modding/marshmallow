# For those who want to use globbing, uncomment the `file` call below,
# and remove the manually entered sources from the `target_sources` call.

#file(GLOB_RECURSE SUBSDK9_SOURCES
#    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/*.s
#    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
#)

target_sources(subsdk9 PRIVATE
        ${SUBSDK9_SOURCES}
        src/main.cpp
)
