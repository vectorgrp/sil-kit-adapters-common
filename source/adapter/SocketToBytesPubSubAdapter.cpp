// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "SocketToBytesPubSubAdapter.hpp"

#include <type_traits>

#include "asio/ts/buffer.hpp"
#include "silkit/services/logging/all.hpp"
#include "silkit/util/serdes/Serialization.hpp"

#include "Exceptions.hpp"
#include "Parsing.hpp"
#include "StringUtils.hpp"

using namespace SilKit::Services::PubSub;
using namespace std::chrono_literals;
using namespace adapters;
using namespace adapters::bytes_socket;

template <bool debug_activated>
void SocketToBytesPubSubAdapter::DoReceiveFrameFromSocket()
{
    static constexpr size_t array_length_size = sizeof(uint32_t);
    static constexpr size_t usable_size = sizeof(_data_buffer_toPublisher) - array_length_size;
    _socket.async_read_some(
        // Setting up the buffer given to point after `array_length_size` bytes to prevent reallocation.
        // This skipped part will contain the SIL Kit-serialized array length.
        asio::buffer(_data_buffer_toPublisher.data() + array_length_size, usable_size),
        [this](const std::error_code ec, const std::size_t bytes_received) {
        if (ec)
            throw IncompleteReadError{};

        // Templating will optimize this (including string building) out for cases when debug
        //  is not configured.
        if (debug_activated)
        {
            _logger->Debug("Adapter >> SIL Kit: "
                           + string(reinterpret_cast<const char*>(_data_buffer_toPublisher.data() + array_length_size),
                                    bytes_received));
        }

        _serializer.BeginArray(bytes_received);
        auto serializer_buffer_out = _serializer.ReleaseBuffer();
        memcpy(_data_buffer_toPublisher.data(), serializer_buffer_out.data(), serializer_buffer_out.size());

        //debug-only test to catch changes in binary implementation of SIL Kit
        assert(array_length_size == serializer_buffer_out.size());

        _publisher->Publish(
            SilKit::Util::Span<uint8_t>(_data_buffer_toPublisher.data(), bytes_received + array_length_size));

        DoReceiveFrameFromSocket<debug_activated>();
    });
}

SocketToBytesPubSubAdapter::SocketToBytesPubSubAdapter(asio::io_context& io_context, const string& host,
                                                       const string& service, const string& publisherName,
                                                       const string& subscriberName, const PubSubSpec& pubDataSpec,
                                                       const PubSubSpec& subDataSpec, SilKit::IParticipant* participant)
    : _socket{io_context}
    , _logger{participant->GetLogger()}
    , _publisher{participant->CreateDataPublisher(publisherName, pubDataSpec)}
    , _subscriber{participant->CreateDataSubscriber(
          subscriberName, subDataSpec,
          _logger->GetLogLevel() <= SilKit::Services::Logging::Level::Debug
              ? DataMessageHandler{[&](IDataSubscriber*, const DataMessageEvent& dataMessageEvent) {
                    _deserializer.Reset(std::move(ToStdVector(dataMessageEvent.data)));
                    this->_data_buffer_fromSubscriber.resize(_deserializer.BeginArray());
                    for (auto& val : _data_buffer_fromSubscriber)
                    {
                        val = _deserializer.Deserialize<uint8_t>(8);
                    }
                    _logger->Debug(
                        "SIL Kit >> Adapter: "
                        + string((const char*)_data_buffer_fromSubscriber.data(), _data_buffer_fromSubscriber.size()));
                    _socket.write_some(
                        asio::buffer(_data_buffer_fromSubscriber.data(), _data_buffer_fromSubscriber.size()));
                }}
              : DataMessageHandler{[&](IDataSubscriber*, const DataMessageEvent& dataMessageEvent) {
                    _deserializer.Reset(std::move(ToStdVector(dataMessageEvent.data)));
                    this->_data_buffer_fromSubscriber.resize(_deserializer.BeginArray());
                    for (auto& val : _data_buffer_fromSubscriber)
                    {
                        val = _deserializer.Deserialize<uint8_t>(8);
                    }
                    _socket.write_some(
                        asio::buffer(_data_buffer_fromSubscriber.data(), _data_buffer_fromSubscriber.size()));
                }})}
{
    try
    {
        asio::connect(_socket, asio::ip::tcp::resolver{io_context}.resolve(host, service));
    }
    catch (std::exception& e)
    {
        std::ostringstream error_message;
        error_message << e.what() << std::endl;
        error_message << "Error encountered while trying to connect to socket at \"" << host << ':' << service << '"';
        throw std::runtime_error(error_message.str());
    }
    _logger->Info("Socket connect success");
    if (_logger->GetLogLevel() <= SilKit::Services::Logging::Level::Debug)
    {
        DoReceiveFrameFromSocket<true>();
    }
    else
    {
        DoReceiveFrameFromSocket<false>();
    }
}

using string = std::string;

/// <summary>
/// parses topicname for format "[ns::]topicname[~defaultname]"
/// leaves topicname without extra said format after execution.
/// </summary>
/// <param name="topicname">input/output</param>
/// <param name="defaultname">output</param>
/// <param name="ns">output</param>
void extractAndEraseNamespaceAndDefaultnameFrom(string& topicname, string& defaultname, string& ns)
{
    //reject mandatory labels as topic names:
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
};

/// <summary>
/// easier to read version from std::count(s.begin(), s.end(), c);
/// </summary>
template <class iterable>
auto count(const iterable& s, char c) ->
    typename std::iterator_traits<typename iterable::const_iterator>::difference_type
{
    return std::count(s.begin(), s.end(), c);
};

void extractTopicLabels(const std::vector<string>& args, std::vector<string>::iterator& arg_iter,
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

string generateSubscriberNameFrom(const string& participantName)
{
    static const string base = participantName + "_sub_";
    static int count = 1;
    return base + std::to_string(count++);
}

string generatePublisherNameFrom(const string& participantName)
{
    static const string base = participantName + "_pub_";
    static int count = 1;
    return base + std::to_string(count++);
}

string SocketToBytesPubSubAdapter::printArgumentHelp(const string& prefix)
{
    return prefix + " <host>:<port>,\n" + prefix + "[<namespace>::]<toAdapter topic name>[~<subscriber's name>]\n"
           + prefix + "   [[,<label key>:<optional label value>\n" + prefix
           + "    |,<label key>=<mandatory label value>\n" + prefix + "   ]],\n" + prefix
           + "[<namespace>::]<fromAdapter topic name>[~<publisher's name>]\n" + prefix
           + "   [[,<label key>:<optional label value>\n" + prefix + "    |,<label key>=<mandatory label value>\n"
           + prefix + "   ]]\n";
}

SocketToBytesPubSubAdapter* SocketToBytesPubSubAdapter::parseArgument(
    char* chardevSocketTransmitterArg, std::set<string>& alreadyProvidedSockets, const string& participantName,
    asio::io_context& ioContext, SilKit::IParticipant* participant, SilKit::Services::Logging::ILogger* logger)
{
    SocketToBytesPubSubAdapter* newAdapter;
    auto args = util::split(chardevSocketTransmitterArg, ",");
    auto arg_iter = args.begin();

    //handle <address>:<port>
    util::assertAdditionalIterator(arg_iter, args);
    throwInvalidCliIf(alreadyProvidedSockets.insert(*arg_iter).second == false);
    auto portAddress = util::split(*arg_iter++, ":");
    throwInvalidCliIf(portAddress.size() != 2);
    const auto& address = portAddress[0];
    const auto& port = portAddress[1];

    //handle inbound topic and labels
    util::assertAdditionalIterator(arg_iter, args);
    string subscriberName = "";
    string subscriberNamespace = "";
    extractAndEraseNamespaceAndDefaultnameFrom(*arg_iter, subscriberName, subscriberNamespace);
    PubSubSpec subDataSpec(*arg_iter++, SilKit::Util::SerDes::MediaTypeData());
    if (subscriberNamespace != "")
        subDataSpec.AddLabel("Namespace", subscriberNamespace, SilKit::Services::MatchingLabel::Kind::Mandatory);

    extractTopicLabels(args, arg_iter, subDataSpec);

    //handle outbound topic
    util::assertAdditionalIterator(arg_iter, args);
    string publisherName = "";
    string publisherNamespace = "";
    extractAndEraseNamespaceAndDefaultnameFrom(*arg_iter, publisherName, publisherNamespace);
    PubSubSpec pubDataSpec(*arg_iter++, SilKit::Util::SerDes::MediaTypeData());
    if (publisherNamespace != "")
        pubDataSpec.AddLabel("Namespace", publisherNamespace, SilKit::Services::MatchingLabel::Kind::Optional);

    extractTopicLabels(args, arg_iter, pubDataSpec);

    throwInvalidCliIf(arg_iter != args.end());

    //generate publisher and subscriber names if not given
    if (subscriberName == "")
        subscriberName = generateSubscriberNameFrom(participantName);
    if (publisherName == "")
        publisherName = generatePublisherNameFrom(participantName);
    newAdapter = new SocketToBytesPubSubAdapter(ioContext, address, port, publisherName, subscriberName, pubDataSpec,
                                                subDataSpec, participant);

    logger->Debug("Created Bytes-PubSub transmitter " + address + ':' + port + " <" + subscriberName + '('
                  + subDataSpec.Topic() + ')' + " >" + publisherName + '(' + pubDataSpec.Topic() + ')');

    return newAdapter;
}
