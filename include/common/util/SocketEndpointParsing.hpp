// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>

#include "Parsing.hpp"
#include "StringUtils.hpp"

namespace adapters { namespace util_socket {

inline void extractTcpSocket(std::string& address, std::string& port, std::vector<std::string>::iterator& arg_iter)
{
    auto hostPort = util::split(*arg_iter++, ":");
    throwInvalidCliIf(hostPort.size() != 2);
    address = hostPort[0];
    port = hostPort[1];
}

inline void extractUnixSocket(std::string& address, std::string& port, std::vector<std::string>::iterator& arg_iter)
{
    address = *arg_iter++;
    port = "-1"; // sentinel for UDS
}

} } // namespace adapters::util_socket
