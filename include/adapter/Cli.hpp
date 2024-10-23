// Copyright (c) Vector Informatik GmbH. All rights reserved.

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