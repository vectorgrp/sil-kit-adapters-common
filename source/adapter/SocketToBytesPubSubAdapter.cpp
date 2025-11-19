// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "SocketToBytesPubSubAdapter.hpp"

#include <type_traits>
#include <csignal>

#include "asio/ts/buffer.hpp"
#include "asio/local/stream_protocol.hpp"
#include "asio/ip/tcp.hpp"

#include "silkit/services/logging/all.hpp"
#include "silkit/util/serdes/Serialization.hpp"

#include "Exceptions.hpp"
#include "Parsing.hpp"
#include "StringUtils.hpp"
#include "common/util/SocketEndpointParsing.hpp"
#include "common/util/PubSubTopicParsing.hpp"

using namespace SilKit::Services::PubSub;
using namespace std::chrono_literals;
using namespace adapters;
using namespace bytes_socket;

template <bool debug_activated>
void SocketToBytesPubSubAdapter::DoReceiveFrameFromSocket()
{
    static constexpr size_t usable_size = sizeof(_data_buffer_toPublisher) - SilKitHeaderSize;
    _socket.async_read_some(
        // Setting up the buffer given to point after `MaxHeaderSize` bytes to prevent reallocation.
        // This skipped part will contain the SIL Kit-serialized array length.
        asio::buffer(_data_buffer_toPublisher.data() + SilKitHeaderSize, usable_size),
        [this](const std::error_code ec, const std::size_t bytes_received) {
        if (ec)
        {
            ShutdownAdapter("Socket read error while reading bytes (code=" + std::to_string(ec.value())
                            + ", msg=" + ec.message() + ")");
            return;
        }

        if (debug_activated)
        {
            _logger->Debug(
                "Adapter >> SIL Kit: "
                + std::string(reinterpret_cast<const char*>(_data_buffer_toPublisher.data() + SilKitHeaderSize),
                              bytes_received));
        }
        SerializeAndPublish(_data_buffer_toPublisher.data(), bytes_received);
        DoReceiveFrameFromSocket<debug_activated>();
    });
}

SocketToBytesPubSubAdapter::SocketToBytesPubSubAdapter(asio::io_context& io_context, SilKit::IParticipant* participant,
                                                       const ParsedPubSubConfig& cfg, bool enableDomainSockets)
    : SocketToPubSubAdapter(io_context, participant, cfg.publisherName, cfg.pubSpec, cfg.subscriberName, cfg.subSpec)
{
    try
    {
        if (enableDomainSockets)
        {
            asio::local::stream_protocol::endpoint endpoint(cfg.address);
            _socket.connect(endpoint);
        }
        else
        {
            asio::ip::tcp::resolver resolver(io_context);
            auto endpoints = resolver.resolve(cfg.address, cfg.port);

            asio::ip::tcp::socket temp_socket(io_context);
            asio::connect(temp_socket, endpoints);
            _socket = asio::generic::stream_protocol::socket(std::move(temp_socket));
        }
    }
    catch (std::exception& e)
    {
        std::ostringstream error_message;
        error_message << e.what() << std::endl;
        if (enableDomainSockets)
            error_message << "Error encountered while trying to connect to Unix Domain Socket at \"" << cfg.address
                          << '"';
        else
            error_message << "Error encountered while trying to connect to socket at \"" << cfg.address << ':'
                          << cfg.port << '"';
        throw std::runtime_error(error_message.str());
    }
    _logger->Info("Socket connection successfully established");
    OnSocketConnected();
}

std::unique_ptr<SocketToBytesPubSubAdapter> SocketToBytesPubSubAdapter::parseArgument(
    char* chardevSocketTransmitterArg, std::set<std::string>& alreadyProvidedSockets,
    const std::string& participantName, asio::io_context& ioContext, SilKit::IParticipant* participant,
    SilKit::Services::Logging::ILogger* logger, bool isUnixSocket)
{
    auto cfg = SocketToPubSubAdapter::parseArgument(chardevSocketTransmitterArg, alreadyProvidedSockets,
                                                    participantName, logger, isUnixSocket);

    std::string debugPrefix;
    if (isUnixSocket)
        debugPrefix = "Created Bytes-PubSub transmitter " + cfg.address;
    else
        debugPrefix = "Created Bytes-PubSub transmitter " + cfg.address + ':' + cfg.port;

    auto newAdapter = std::make_unique<SocketToBytesPubSubAdapter>(ioContext, participant, cfg, isUnixSocket);

    std::string debug_message = debugPrefix + " <" + cfg.subscriberName + '(' + cfg.subSpec.Topic() + ')' + " >"
                                + cfg.publisherName + '(' + cfg.pubSpec.Topic() + ')';
    logger->Debug(debug_message);
    return newAdapter;
}

void SocketToBytesPubSubAdapter::WriteOutbound(const uint8_t* data, size_t size)
{
    if (!IsSocketConnected())
    {
        if (_logger && _logger->GetLogLevel() <= SilKit::Services::Logging::Level::Debug)
            _logger->Debug("SocketToBytesPubSubAdapter: outbound dropped (socket not connected)");
        return;
    }
    asio::write(_socket, asio::buffer(data, size));
}

void SocketToBytesPubSubAdapter::StartReceivingFromSocket()
{
    if (_debugEnabled)
        DoReceiveFrameFromSocket<true>();
    else
        DoReceiveFrameFromSocket<false>();
}

void SocketToBytesPubSubAdapter::OnInbound(const SilKit::Services::PubSub::DataMessageEvent& evt)
{
    HandleInboundImpl(_data_buffer_toPublisher, evt);
}