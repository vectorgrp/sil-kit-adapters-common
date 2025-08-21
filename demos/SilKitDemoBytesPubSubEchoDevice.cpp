// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "PubSubEchoDevice.hpp"

int main(int argc, char** argv)
{
    return EchoDemoMain(argc, argv, getArgDefault(argc, argv, participantNameArg, "EchoDevice"),
                        PubSubSpec("fromSocket", SilKit::Util::SerDes::MediaTypeData()),
                        PubSubSpec("toSocket", SilKit::Util::SerDes::MediaTypeData()));
}
