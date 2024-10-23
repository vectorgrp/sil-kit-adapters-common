function( copy_sil_kit_to_bin target_name output_directory library_directory )
    if(WIN32)
        # Copy the SilKit.dll next to the executables
        add_custom_command(
            TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy $<TARGET_FILE:SilKit::SilKit> ${output_directory}/SilKit.dll
        )
        # Copy the SilKit.lib in the lib folder
        get_target_property(SILKIT_LIB SilKit::SilKit LOCATION) # Get the SilKit.dll full path
        get_filename_component(SILKIT_DIR_LIB ${SILKIT_LIB} DIRECTORY) # Keep only the SilKit.dll directory path
        add_custom_command(
            TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy ${SILKIT_DIR_LIB}/../lib/SilKit.lib ${library_directory}/SilKit.lib
        )
    else()
        # Copy the libSilKit.so in the lib folder
        add_custom_command(
            TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy $<TARGET_FILE:SilKit::SilKit> ${library_directory}/libSilKit.so
        )
    endif()
endfunction()