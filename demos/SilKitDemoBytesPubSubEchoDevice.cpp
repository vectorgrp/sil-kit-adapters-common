// Copyright (c) Vector Informatik GmbH. All rights reserved.

#include "PubSubEchoDevice.hpp"
/**************************************************************************************************
 * Main Function
 **************************************************************************************************/


int main(int argc, char** argv)
{    
    return EchoDemoMain(argc, argv, 
        getArgDefault(argc, argv, participantNameArg, "EchoDevice"),
        PubSubSpec("fromSocket", SilKit::Util::SerDes::MediaTypeData()),
        PubSubSpec("toSocket", SilKit::Util::SerDes::MediaTypeData()));
}
