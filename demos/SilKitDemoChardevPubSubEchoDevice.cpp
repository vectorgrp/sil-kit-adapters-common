// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "PubSubEchoDevice.hpp"

int main(int argc, char** argv)
{
    const std::string participantName = getArgDefault(argc, argv, participantNameArg, "EchoDevice");

    const auto create_pubsubspec = [](const std::string& topic_name,
                                      SilKit::Services::MatchingLabel::Kind matching_mode) {
        PubSubSpec r(topic_name, SilKit::Util::SerDes::MediaTypeData());
        r.AddLabel("VirtualNetwork", "Default", matching_mode);
        r.AddLabel("Namespace", "Namespace", matching_mode);
        return r;
    };

    PubSubSpec subDataSpec = create_pubsubspec("fromChardev", SilKit::Services::MatchingLabel::Kind::Mandatory);
    subDataSpec.AddLabel("Instance", "Adapter", SilKit::Services::MatchingLabel::Kind::Mandatory);

    PubSubSpec pubDataSpec = create_pubsubspec("toChardev", SilKit::Services::MatchingLabel::Kind::Optional);
    pubDataSpec.AddLabel("Instance", participantName, SilKit::Services::MatchingLabel::Kind::Optional);

    return EchoDemoMain(argc, argv, participantName, subDataSpec, pubDataSpec);
}
