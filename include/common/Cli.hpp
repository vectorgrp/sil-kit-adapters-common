// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

namespace adapters {

enum ReturnCode
{
#ifndef WIN32 // already defined in winerror.h
    NO_ERROR = 0,
#endif
    CodeSuccess = 0,
    CodeErrorCli,
    CodeErrorConfiguration,
    CodeErrorOther,
    CodeErrorFileDescriptor = -1
};

void promptForExit();

} // namespace adapters