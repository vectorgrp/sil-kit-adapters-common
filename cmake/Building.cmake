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