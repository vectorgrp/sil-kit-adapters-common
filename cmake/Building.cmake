# SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
# SPDX-License-Identifier: MIT

function(copy_sil_kit_to_bin target_name output_directory library_directory)
    get_target_property(SILKIT_DIR_LIB SilKit::SilKit LOCATION) # Get the SilKit.dll full path
    get_filename_component(SILKIT_DIR_LIB ${SILKIT_DIR_LIB} DIRECTORY) # Get the SilKit.dll directory path

    if(WIN32)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            set(SILKIT_DLL_NAME "SilKit.dll")
            set(SILKIT_LIB_NAME "SilKit.lib")
        else() # Debug or RelWithDebInfo
            set(SILKIT_DLL_NAME "SilKitd.dll")
            set(SILKIT_LIB_NAME "SilKitd.lib")
        endif()
        # Copy the SilKit.dll next to the executables
        add_custom_command(
            TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy ${SILKIT_DIR_LIB}/${SILKIT_DLL_NAME} ${output_directory}/${SILKIT_DLL_NAME}
        )
        # Copy the SilKit(d).lib in the lib folder
        add_custom_command(
            TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy ${SILKIT_DIR_LIB}/../lib/${SILKIT_LIB_NAME} ${library_directory}/${SILKIT_LIB_NAME}
        )
    else()
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            set(SILKIT_SO_NAME "libSilKit.so")
        else() # Debug or RelWithDebInfo
            set(SILKIT_SO_NAME "libSilKitd.so")
        endif()
        # Copy the libSilKit.so in the lib folder
        add_custom_command(
            TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy ${SILKIT_DIR_LIB}/${SILKIT_SO_NAME} ${library_directory}/${SILKIT_SO_NAME}
        )
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
