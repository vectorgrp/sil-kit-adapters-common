# SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
# SPDX-License-Identifier: MIT

# SIL KIT PACKAGE CONSUMPTION

## On Windows, if an msi-installed version of SIL Kit is available and you want to build the adapters against it, CMAKE will attempt to use it.

## If you want to use another specific SIL Kit package (downloaded Release package OR self-built & installed package), you must specify the path to it by setting SILKIT_PACKAGE_DIR
## if it is a downloaded Release package of SIL Kit:
##      "cmake -S. -Bbuild -DSILKIT_PACKAGE_DIR=/path/to/SilKit-x.y.z-Platform/"
## if it is a self-build and installed package of SIL Kit:
##      "cmake -S. -Bbuild -DSILKIT_PACKAGE_DIR=/path/to/self-installed/package/"

# Required minimal SIL Kit version during lookup.
set(SILKIT_REQUIRED_MIN_VERSION "4.0.40")

set(SILKIT_DEFAULT_DOWNLOAD_VERSION "5.0.0" CACHE STRING "If no SIL Kit package is specified with SILKIT_VERSION, this package version will be downloaded")
set(SILKIT_DEFAULT_DOWNLOAD_FLAVOR_NONWIN "ubuntu-20.04-x86_64-gcc" CACHE STRING "If no SIL Kit package is specified, this package flavor will be downloaded on non-windows system.")
set(SILKIT_DEFAULT_DOWNLOAD_FLAVOR_WIN "Win-x86_64-VS2019" CACHE STRING "If no SIL Kit package is specified, this package flavor will be downloaded on Windows systems.")

if((NOT DEFINED SILKIT_FLAVOR))
    if (WIN32)
        set(SILKIT_FLAVOR_D "${SILKIT_DEFAULT_DOWNLOAD_FLAVOR_WIN}")
    else()
        set(SILKIT_FLAVOR_D "${SILKIT_DEFAULT_DOWNLOAD_FLAVOR_NONWIN}")
    endif()
    set(SILKIT_FLAVOR "${SILKIT_FLAVOR_D}" CACHE STRING "Overrideable flavor of SilKit for download." FORCE)
endif()

if(DEFINED SILKIT_PACKAGE_DIR)
    # if the user forces using a specific SIL Kit package, use it
    message(STATUS "SILKIT_PACKAGE_DIR has been set to: ${SILKIT_PACKAGE_DIR}, CMAKE will look for a SIL Kit package in that directory")
    find_package(SilKit ${SILKIT_REQUIRED_MIN_VERSION}
        REQUIRED
        CONFIG
        NO_CMAKE_PACKAGE_REGISTRY
        NO_DEFAULT_PATH
        PATHS "${SILKIT_PACKAGE_DIR}"
    )
    message(STATUS "Using SIL Kit package from user-specified directory: ${SILKIT_PACKAGE_DIR}")
else()
    if (WIN32)
        # otherwise, look for an installed version of SIL Kit (.msi file)
        message(STATUS "SILKIT_PACKAGE_DIR has not been set by user. Attempting to find an msi-installed version of SIL Kit")
        find_package(SilKit ${SILKIT_REQUIRED_MIN_VERSION}
        CONFIG)
        if(SilKit_FOUND)
            message(STATUS "Using msi-installed SIL Kit package.")
        endif()
    endif()

    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24.0")
        set(FetchContent_Declare_TIMESTAMPS DOWNLOAD_EXTRACT_TIMESTAMP true)
    endif()
  
    if(NOT SilKit_FOUND)
        message(STATUS "No version of SIL Kit present. Attempting to fetch [SilKit-${SILKIT_DEFAULT_DOWNLOAD_VERSION}-${SILKIT_FLAVOR}] from github.com")
        include(FetchContent)
        FetchContent_Declare(
            silkit
            ${FetchContent_Declare_TIMESTAMPS}
            URL https://github.com/vectorgrp/sil-kit/releases/download/v${SILKIT_DEFAULT_DOWNLOAD_VERSION}/SilKit-${SILKIT_DEFAULT_DOWNLOAD_VERSION}-${SILKIT_FLAVOR}.zip
            DOWNLOAD_DIR ${CMAKE_CURRENT_LIST_DIR}/Downloads
        )

        message(STATUS "SIL Kit: fetching [SilKit-${SILKIT_DEFAULT_DOWNLOAD_VERSION}-${SILKIT_FLAVOR}]")
        FetchContent_MakeAvailable(silkit)

        set(SILKIT_SOURCE_DIR ${silkit_SOURCE_DIR}/SilKit-Source)
        message(STATUS "SIL Kit: using source code from: \"${SILKIT_SOURCE_DIR}\"")
        message(STATUS "SIL Kit: using pre-built binaries from: ${silkit_SOURCE_DIR}")

        message(STATUS "Searching SilKit package which has been fetched from github.com")
        find_package(SilKit ${SILKIT_REQUIRED_MIN_VERSION}
            REQUIRED
            CONFIG
            NO_CMAKE_PACKAGE_REGISTRY
            NO_DEFAULT_PATH
            PATHS "${silkit_SOURCE_DIR}"
        )
        message(STATUS "Using SIL Kit package fetched from GitHub: ${silkit_SOURCE_DIR}")
    endif()  
endif()

if(NOT TARGET SilKit::SilKit)
    message(FATAL_ERROR "Something went wrong: Could not find SIL Kit package.")
else()
    if(SilKit_VERSION)
        message(STATUS "SIL Kit version being used: ${SilKit_VERSION}")
    else()
        message(WARNING "SIL Kit version information is not available.")
    endif()
    message(STATUS "SIL Kit package successfully found and configured.")
endif()