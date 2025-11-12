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

function(install_adapter_source)
    set(directory_paths ${ARGV})
    install(
        DIRECTORY
            ${directory_paths}

        DESTINATION .
        COMPONENT source
        EXCLUDE_FROM_ALL

        FILES_MATCHING
            PATTERN *.cpp
            PATTERN *.hpp
            PATTERN *.cmake
            PATTERN *.ps1
            PATTERN *.sh
            PATTERN *.json
            PATTERN *.yaml
            PATTERN *.yml
            PATTERN *.can
            PATTERN *.dbc
            PATTERN *.cfg
            PATTERN *.vcdl
            PATTERN *.vCODM
            PATTERN *.xvp
            PATTERN *.xml
            PATTERN *.fbbin
            PATTERN *.md
            PATTERN *.rst
            PATTERN *.png
            PATTERN *.1
            PATTERN Dockerfile
            PATTERN LICENSE
            PATTERN CMakeLists.txt

        REGEX "\.git$" EXCLUDE
        REGEX "\.github$" EXCLUDE
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

function(install_lib_debug debug)
    install(
        FILES
            ${debug}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT debug
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
    
    get_target_property(SILKIT_DLL_DIR SilKit::SilKit LOCATION)
    get_filename_component(SILKIT_DLL_DIR ${SILKIT_DLL_DIR} DIRECTORY)
    get_filename_component(SILKIT_LIB_DIR "${SILKIT_DLL_DIR}/../lib" REALPATH)

    if(WIN32)
        # Collect all .dll files from both DLL and LIB directories
        file(GLOB SILKIT_DLLS "${SILKIT_DLL_DIR}/*.dll")
        foreach(dll ${SILKIT_DLLS})
            install_lib_windows(${dll})
        endforeach()

        # Collect and install .pdb files manually
        file(GLOB SILKIT_PDBS "${SILKIT_LIB_DIR}/*.pdb")
        foreach(pdb ${SILKIT_PDBS})
            install_lib_debug(${pdb})
        endforeach()

    else()
        # Collect and install .so and .debug files
        file(GLOB SILKIT_SOS "${SILKIT_LIB_DIR}/*.so")
        foreach(so ${SILKIT_SOS})
            install_lib_linux(${so})
        endforeach()
    
        file(GLOB SILKIT_DEBUGS "${SILKIT_LIB_DIR}/*.debug")
        foreach(debug ${SILKIT_DEBUGS})
            install_lib_debug(${debug})
        endforeach()
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

function(fix_windows_install_folder adapter_name adapter_version)
    if(WIN32)
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            # set the default install paths for Windows 32 and 64 bits
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(CMAKE_INSTALL_PREFIX "C:/Program Files/Vector ${adapter_name} ${adapter_version}" CACHE PATH "Default install path" FORCE)
            elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
                set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Vector ${adapter_name} ${adapter_version}" CACHE PATH "Default install path" FORCE)
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
