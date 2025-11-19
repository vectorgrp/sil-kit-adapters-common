// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <set>

#include "asio/ts/net.hpp"
#include "asio/generic/stream_protocol.hpp"

#include "silkit/SilKit.hpp"
#include "silkit/services/pubsub/all.hpp"

#include "SocketToPubSubAdapter.hpp"

namespace asio {
class io_context;
}

namespace adapters {

namespace bytes_socket {

class SocketToBytesPubSubAdapter : public SocketToPubSubAdapter
{
    typedef std::string string;
    typedef SilKit::Services::PubSub::PubSubSpec PubSubSpec;

public:
    using ParsedPubSubConfig = SocketToPubSubAdapter::ParsedPubSubConfig;
    SocketToBytesPubSubAdapter(asio::io_context& io_context, SilKit::IParticipant* participant,
                               const ParsedPubSubConfig& cfg, bool enableDomainSockets);

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
    static std::unique_ptr<SocketToBytesPubSubAdapter> parseArgument(
        char* chardevSocketTransmitterArg, std::set<std::string>& alreadyProvidedSockets,
        const std::string& participantName, asio::io_context& ioContext, SilKit::IParticipant* participant,
        SilKit::Services::Logging::ILogger* logger, bool isUnixSocket);

    void WriteOutbound(const uint8_t* data, size_t size) override;
    void StartReceivingFromSocket() override;
    void OnInbound(const SilKit::Services::PubSub::DataMessageEvent& evt) override;

private:
    template <bool debug_activated>
    void DoReceiveFrameFromSocket();

private:
    std::array<uint8_t, 4096> _data_buffer_toPublisher{};
};

} // namespace bytes_socket
} // namespace adapters