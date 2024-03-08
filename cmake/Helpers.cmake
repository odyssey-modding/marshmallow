function(generate_npdm target json)
    add_custom_target(${target}_npdm
            DEPENDS ${target}
            COMMAND ${CMAKE_SOURCE_DIR}/tools/npdmtool${EXE_EXT} ${json} ${CMAKE_BINARY_DIR}/${target}.npdm
            BYPRODUCTS ${CMAKE_BINARY_DIR}/${target}.npdm
    )
endfunction()

function(make_nso_target target)
    add_custom_target(${target}_nso
            DEPENDS ${target}
            COMMAND ${CMAKE_SOURCE_DIR}/tools/linkle nso ${CMAKE_BINARY_DIR}/${target}.elf ${CMAKE_BINARY_DIR}/${target}
    )
endfunction()

function(deploy_nso target)
    if (NOT TARGET ${target}_nso)
        message(FATAL_ERROR "Target ${target} does not exist")
    endif ()
    if (NOT TARGET ${target}_npdm)
        message(FATAL_ERROR "NPDM for ${target} does not exist")
    endif ()
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/deploy/${target})
    add_custom_target(${target}_deploy ALL
            DEPENDS ${target}_nso ${target}_npdm
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${target} ${CMAKE_SOURCE_DIR}/deploy/${target}/
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${target}.npdm ${CMAKE_SOURCE_DIR}/deploy/${target}/main.npdm
    )
endfunction()

function(deploy_nso_ftp target title_id)
    if (NOT TARGET ${target}_deploy)
        deploy_nso(${target})
    endif ()
    if (FTP_IP)
        set(FTP_PORT 5000 CACHE STRING "FTP Port")
        set(FTP_USERNAME anonymous CACHE STRING "FTP Username")
        set(FTP_PASSWORD anonymous CACHE STRING "FTP Password")
        file(GLOB_RECURSE _FTP_FILES ${CMAKE_SOURCE_DIR}/deploy/${target}/*)
        list(JOIN _FTP_FILES "," _FTP_FILES_JOINED)
        add_custom_target(${target}_deploy_ftp ALL
                DEPENDS ${target}_deploy
                COMMAND ${CURL_LOCATION}
                    --ftp-create-dirs -T
                    "{${_FTP_FILES_JOINED}}"
                    ftp://${FTP_USERNAME}:${FTP_PASSWORD}@${FTP_IP}:${FTP_PORT}/atmosphere/contents/${title_id}/exefs/
        )
    else ()
    endif ()
endfunction()
