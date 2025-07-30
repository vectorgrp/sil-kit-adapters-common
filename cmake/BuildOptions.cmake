# SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
# SPDX-License-Identifier: MIT

option(ENABLE_ASAN "Enable -f sanitize=address for builds (requires gcc, clang,  VS2019)" OFF)
option(ENABLE_UBSAN "Enable -f sanitize=undefined for builds (requires gcc, clang)" OFF)
option(ENABLE_THREADSAN "Enable -f sanitize=thread for builds (requires gcc, clang)" OFF)
