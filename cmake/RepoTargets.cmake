# SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.12)

include(${CMAKE_CURRENT_LIST_DIR}/BuildProfile.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/BuildOptions.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/Prerequisites.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/FindSilKit.cmake)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../source/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../demos/)
