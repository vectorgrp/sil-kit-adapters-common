// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <set>
#include "asio/ts/net.hpp"
#include "silkit/SilKit.hpp"
#include "silkit/services/pubsub/all.hpp"
#include "silkit/util/serdes/Deserializer.hpp"
#include "silkit/util/serdes/Serializer.hpp"

namespace asio {
class io_context;
}

namespace adapters {
namespace bytes_socket {

class SocketToBytesPubSubAdapter
{
    typedef std::string string;
    typedef SilKit::Services::PubSub::PubSubSpec PubSubSpec;

public:
    SocketToBytesPubSubAdapter(asio::io_context& io_context, const string& host, const string& service,
        const string& publisherName, const string& subscriberName,
        const PubSubSpec& pubDataSpec, const PubSubSpec& subDataSpec,
        SilKit::IParticipant* participant);

    static string printArgumentHelp(const string& prefix = "    ");

    /// <summary>
    /// Parse the complex string given as the argument requesting an adaptation from a socket to a publisher/subscriber.
    /// </summary>
    /// <param name="bytesSocketTransmitterArg">The argument as input.</param>
    /// <param name="alreadyProvidedSockets">output to log requested sockets.</param>
    /// <param name="participantName">Used for creating default publish/subscrib/er names if none requested.</param>
    /// <param name="ioContext">Passed to constructor of class SocketToBytesPubSubAdapter.</param>
    /// <param name="participant">Passed to constructor of class SocketToBytesPubSubAdapter.</param>
    /// <param name="logger">Used for printing logging info.</param>
    /// <returns>A pointer to the created SocketToBytesPubSubAdapter.</returns>
    static SocketToBytesPubSubAdapter* parseArgument(char* bytesSocketTransmitterArg,
        std::set<string>& alreadyProvidedSockets,
        const string& participantName,
        asio::io_context& ioContext,
        SilKit::IParticipant* participant,
        SilKit::Services::Logging::ILogger* logger);


private:
    //internal callbacks
    template<bool debug_activated>
    void DoReceiveFrameFromSocket();

private:
    asio::ip::tcp::socket _socket;
    SilKit::Services::Logging::ILogger* _logger;
    std::array<uint8_t, 4096> _data_buffer_toPublisher = {};
    std::vector<uint8_t> _data_buffer_fromSubscriber = {};
    SilKit::Services::PubSub::IDataPublisher* _publisher;
    SilKit::Services::PubSub::IDataSubscriber* _subscriber;
    SilKit::Util::SerDes::Serializer _serializer;
    SilKit::Util::SerDes::Deserializer _deserializer;
};

} // namespace bytes_socket
} // namespace adapters
