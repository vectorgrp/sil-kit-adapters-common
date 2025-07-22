# SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
# SPDX-License-Identifier: MIT

################################################################################
# Distribution of the source code and binaries
################################################################################
# Install sources and binaries
# Copy all files from the source and bin directory to the proper destination
# Leave out git repo related data

include(GNUInstallDirs)

function(install_git_source git_directory)
    install(
        DIRECTORY
            ${git_directory}

        DESTINATION .
        COMPONENT source
        EXCLUDE_FROM_ALL
        REGEX "\.git$" EXCLUDE
        REGEX "\.github$" EXCLUDE
        REGEX "Downloads$" EXCLUDE
    )
endfunction()

function(install_adapter_source adapter_directory)
    install(
        DIRECTORY
            ${adapter_directory}

        DESTINATION .
        COMPONENT source
        EXCLUDE_FROM_ALL
        REGEX "Downloads$" EXCLUDE
    )
endfunction()

function(install_root_source_files)
    install(
        FILES
            CMakeLists.txt
            CMakePresets.json
            LICENSE
            README.md
            SECURITY.md
            CONTRIBUTING.md
        DESTINATION .
        COMPONENT source
        EXCLUDE_FROM_ALL
    )
endfunction()

function(install_lib_linux lib_file)
    install(
        FILES
            ${lib_file}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PERMISSIONS
            OWNER_READ OWNER_WRITE
            GROUP_READ
            WORLD_READ
        COMPONENT lib
        EXCLUDE_FROM_ALL
    )
endfunction()

function(install_lib_windows lib_file)
    install(
        FILES
            ${lib_file}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT lib
        EXCLUDE_FROM_ALL
    )
    get_filename_component(LIB_DIR  "${lib_file}" DIRECTORY)
    get_filename_component(LIB_NAME "${lib_file}" NAME_WE)
    install(
        FILES
            ${LIB_DIR}/../lib/${LIB_NAME}.lib
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT lib
        EXCLUDE_FROM_ALL
    )
endfunction()

function(install_silkit_lib output_dir library_dir)
    if(WIN32)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            set(SILKIT_DLL_NAME "SilKit.dll")
        else() # Debug or RelWithDebInfo
            set(SILKIT_DLL_NAME "SilKitd.dll")
        endif()
        install_lib_windows(${output_dir}/${SILKIT_DLL_NAME})    # Set SIL Kit lib name
    else()
        # Set SIL Kit lib name
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            set(SILKIT_SO_NAME "libSilKit.so")
        else() # Debug or RelWithDebInfo
            set(SILKIT_SO_NAME "libSilKitd.so")
        endif()
        install_lib_linux(${library_dir}/${SILKIT_SO_NAME})
    endif()
endfunction()

function(install_demo demo_executable_name)
    install(
        TARGETS 
            ${demo_executable_name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE    
        EXCLUDE_FROM_ALL
    )
endfunction()

function(install_adapter adapter_executable_name)
    install(
        TARGETS
            ${adapter_executable_name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${adapter_executable_name} COMPONENT Development
        PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE
    )
endfunction()

function(fix_windows_install_folder adapter_version)
    if(WIN32)
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            # set the default install paths for Windows 32 and 64 bits
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(CMAKE_INSTALL_PREFIX "C:/Program Files/Vector SIL Kit Adapter Bytestream Socket ${adapter_version}" CACHE PATH "Default install path" FORCE)
            elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
                set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Vector SIL Kit Adapter Bytestream Socket ${adapter_version}" CACHE PATH "Default install path" FORCE)
            endif()
        endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    endif()
endfunction()

function(install_scripts_to where)
  if(${ARGC} GREATER 1)
    set(what ${ARGN})
    install(
        FILES
            ${what}
        DESTINATION ${where}
        PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE
            GROUP_READ GROUP_EXECUTE
            WORLD_READ WORLD_EXECUTE
        COMPONENT source
        EXCLUDE_FROM_ALL
    )
  endif()
endfunction()
