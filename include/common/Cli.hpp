// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once 

namespace adapters {

    enum ReturnCode
    {
        CodeSuccess = 0,
        CodeErrorCli,
        CodeErrorConfiguration,
        CodeErrorOther
    };
    
    void promptForExit();

} // namespace adapters