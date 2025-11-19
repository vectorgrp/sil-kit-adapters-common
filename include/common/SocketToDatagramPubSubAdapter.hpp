// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <set>
#include <vector>

#include "Parsing.hpp"

#include "asio/ts/net.hpp"
#include "asio/generic/stream_protocol.hpp"
#include "asio/write.hpp"
#include "asio/ts/buffer.hpp"

#include "silkit/SilKit.hpp"
#include "silkit/services/pubsub/all.hpp"

#include "common/util/WriteUint.hpp"
#include "SocketToPubSubAdapter.hpp"

namespace asio {
class io_context;
}

namespace adapters {

namespace datagram_socket {

class SocketToDatagramPubSubAdapter : public SocketToPubSubAdapter
{
public:
    using ParsedPubSubConfig = SocketToPubSubAdapter::ParsedPubSubConfig;
    SocketToDatagramPubSubAdapter(asio::io_context& io_context, SilKit::IParticipant* participant,
                                  const ParsedPubSubConfig& cfg, Endianness endianness, uint8_t headerSize);

    /// <summary>
    /// Parse the complex string given as the argument requesting an adaptation from a socket to a publisher/subscriber.
    /// </summary>
    /// <param name="dataGramTransmitterArg">The argument as input.</param>
    /// <param name="alreadyProvidedSockets">output to log requested sockets.</param>
    /// <param name="participantName">Used for creating default publish/subscrib/er names if none requested.</param>
    /// <param name="ioContext">Passed to constructor of class SocketToDatagramPubSubAdapter.</param>
    /// <param name="participant">Passed to constructor of class SocketToDatagramPubSubAdapter.</param>
    /// <param name="endianness">Defines the endianness with which the Datagram header is handled.</param>
    /// <param name="headerSize">The size of the header of the Datagram.</param>
    /// <param name="logger">Used for printing logging info.</param>
    /// <returns>A pointer to the created SocketToDatagramAdapter.</returns>
    static std::unique_ptr<SocketToDatagramPubSubAdapter> parseArgument(
        char* dataGramTransmitterArg, std::set<std::string>& alreadyProvidedSockets, const std::string& participantName,
        asio::io_context& ioContext, SilKit::IParticipant* participant, Endianness endianness, uint8_t headerSize,
        SilKit::Services::Logging::ILogger* logger);

    void WriteOutbound(const uint8_t* data, size_t size) override;
    void StartReceivingFromSocket() override;
    void OnInbound(const SilKit::Services::PubSub::DataMessageEvent& evt) override;

private:
    //internal callbacks
    template <bool debug_activated>
    void DoReceiveFrameFromSocket();

private:
    uint8_t _headerSize; // number of bytes used to encode frame size
    Endianness _endianness;
    std::array<uint8_t, 4> _frame_size_buffer = {};
    std::vector<uint8_t> _data_buffer_toPublisher;
};

} // namespace datagram_socket
} // namespace adapters