// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <vector>
#include <set>
#include <iterator>
#include <algorithm>

#include "Parsing.hpp"
#include "StringUtils.hpp"
#include "silkit/services/pubsub/all.hpp"

namespace adapters { namespace util_topic {

using string = std::string;

// Minimal helper to count occurrences of a character in a string.
// Restores expected functionality for extractTopicLabels without altering its logic.
/// <summary>
/// easier to read version from std::count(s.begin(), s.end(), c);
/// </summary>
template <class iterable>
auto count(const iterable& s, char c) ->
    typename std::iterator_traits<typename iterable::const_iterator>::difference_type
{
    return std::count(s.begin(), s.end(), c);
};

inline void extractAndEraseNamespaceAndDefaultnameFrom(string& topicname, string& defaultname, string& ns)
{
    throwInvalidCliIf(topicname.find_first_of("=") != string::npos);
    auto splitTopic = util::split(topicname, "~");
    if (splitTopic.size() == 2)
    {
        defaultname = splitTopic[1];
        throwInvalidCliIf(defaultname == "");
        topicname = splitTopic[0];
    }
    else if (splitTopic.size() != 1)
    {
        throw InvalidCli();
    }
    splitTopic = util::split(topicname, ":");
    if (splitTopic.size() == 3 && splitTopic[1] == "")
    {
        ns = splitTopic[0];
        throwInvalidCliIf(ns == "");
        topicname = splitTopic[2];
        return;
    }
    else if (splitTopic.size() == 1)
    {
        return;
    }
    else
    {
        throw InvalidCli();
    }
}

inline void extractTopicLabels(const std::vector<string>& args, std::vector<string>::iterator& arg_iter,
                        SilKit::Services::PubSub::PubSubSpec& dataSpec)
{
    for (; arg_iter != args.end() && (count(*arg_iter, ':') == 1 || count(*arg_iter, '=') == 1); ++arg_iter)
    {
        auto labelKeyValue = util::split(*arg_iter, ":=");
        dataSpec.AddLabel(labelKeyValue[0], labelKeyValue[1], [&]() -> auto {
            if ((*arg_iter)[labelKeyValue[0].size()] == '=')
                return SilKit::Services::MatchingLabel::Kind::Mandatory;
            else
                return SilKit::Services::MatchingLabel::Kind::Optional;
        }());
    }
};

inline string generateSubscriberNameFrom(const string& participantName)
{
    static int count = 1;
    return participantName + "_sub_" + std::to_string(count++);
}

inline string generatePublisherNameFrom(const string& participantName)
{
    static int count = 1;
    return participantName + "_pub_" + std::to_string(count++);
}

} } // namespace adapters::util_topic
