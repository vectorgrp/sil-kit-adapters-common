# SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
# SPDX-License-Identifier: MIT

set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")

if(CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Reminder: Use --config <Release|Debug> when building with multi-config generators like Visual Studio.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    message(WARNING "CMAKE_BUILD_TYPE was not set. Defaulting to 'Release'. You can override with -DCMAKE_BUILD_TYPE=<Release|Debug>.")
endif()
