# SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
# SPDX-License-Identifier: MIT
function(copy_sil_kit_to_bin target_name output_directory library_directory)
    get_target_property(SILKIT_DLL_DIR SilKit::SilKit LOCATION)
    get_filename_component(SILKIT_DLL_DIR ${SILKIT_DLL_DIR} DIRECTORY)
    get_filename_component(SILKIT_LIB_DIR "${SILKIT_DLL_DIR}/../lib" REALPATH)
    
    if(WIN32)
        # Collect all .dll, .lib, .pdb from bin and lib
        file(GLOB WIN_BIN_FILES
            "${SILKIT_DLL_DIR}/*.dll"
            "${SILKIT_LIB_DIR}/*.dll"
        )
        file(GLOB WIN_LIB_FILES
            "${SILKIT_LIB_DIR}/*.lib"
            "${SILKIT_LIB_DIR}/*.pdb"
        )

        # Copy all bin files to output_directory
        foreach(f ${WIN_BIN_FILES})
            get_filename_component(fname "${f}" NAME)
            add_custom_command(
                TARGET ${target_name}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND}
                ARGS -E copy "${f}" "${output_directory}/${fname}"
            )
        endforeach()

        # Copy all lib files to library_directory
        foreach(f ${WIN_LIB_FILES})
            get_filename_component(fname "${f}" NAME)
            add_custom_command(
                TARGET ${target_name}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${f}" "${library_directory}/${fname}"
            )
        endforeach()
    else()
        # Collect all .so and .debug files from lib
        file(GLOB LINUX_LIB_FILES
            "${SILKIT_LIB_DIR}/*.so"
            "${SILKIT_LIB_DIR}/*.debug"
        )
        foreach(f ${LINUX_LIB_FILES})
            get_filename_component(fname "${f}" NAME)
            add_custom_command(
                TARGET ${target_name}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${f}" "${library_directory}/${fname}"
            )
        endforeach()
    endif()
endfunction()

function(enable_address_sanitizer ENABLE_ASAN)
    if (ENABLE_ASAN)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
endfunction()

function(enable_undefined_sanitizer ENABLE_UBSAN)
    if (ENABLE_UBSAN)
        add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=undefined)
    endif()
endfunction()

function(enable_thread_sanitizer ENABLE_THREADSAN)
    if (ENABLE_THREADSAN)
        add_compile_options(-fsanitize=thread -fno-omit-frame-pointer -g3)
        add_link_options(-fsanitize=thread -fno-omit-frame-pointer -g3)
    endif()
endfunction()
