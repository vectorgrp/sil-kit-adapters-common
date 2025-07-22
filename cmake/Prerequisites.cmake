# SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
# SPDX-License-Identifier: MIT

# Finding prerequisites

## Finding asio

### Finding Threads
find_package(Threads REQUIRED)

add_library(asio INTERFACE)
target_include_directories(asio SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../third_party/asio/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE=1)
target_link_libraries(asio INTERFACE Threads::Threads)
if(WIN32 )
    target_compile_definitions(asio INTERFACE _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING=1)
    target_compile_definitions(asio INTERFACE WINVER=0x0601 _WIN32_WINNT=0x0601) # Windows 7
endif()