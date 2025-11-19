// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "SocketToDatagramPubSubAdapter.hpp"

#include "asio/ts/buffer.hpp"
#include "asio/local/stream_protocol.hpp"
#include "asio/ip/tcp.hpp"

#include "silkit/services/logging/all.hpp"
#include "silkit/util/serdes/Serialization.hpp"

#include "Exceptions.hpp"
#include "Parsing.hpp"
#include "StringUtils.hpp"
#include "common/util/PubSubTopicParsing.hpp"
#include "common/util/SocketEndpointParsing.hpp"

using namespace SilKit::Services::PubSub;
using namespace std::chrono_literals;
using namespace adapters;
using namespace datagram_socket;

template <bool debug_activated>
void SocketToDatagramPubSubAdapter::DoReceiveFrameFromSocket()
{
    if (_headerSize == 0 || _headerSize > SilKitHeaderSize)
    {
        ShutdownAdapter("Invalid headerSize=" + std::to_string(_headerSize) + " (must be 1..4)");
        return;
    }

    asio::async_read(_socket, asio::buffer(_frame_size_buffer.data(), _headerSize),
                     [this](std::error_code ec, std::size_t nReadHeader) {
        if (ec || nReadHeader != _headerSize)
        {
            ShutdownAdapter("Socket read error while reading length header (code=" + std::to_string(ec.value())
                            + ", msg=" + ec.message() + ")");
            return;
        }

        uint32_t frame_size = 0;
        for (unsigned i = 0; i < _headerSize; ++i)
        {
            unsigned srcIndex = (_endianness == Endianness::big_endian) ? (_headerSize - 1 - i) : i;
            frame_size |= static_cast<uint32_t>(_frame_size_buffer[srcIndex]) << (8u * i);
        }

        // Ensure buffer capacity (prefix + payload)
        _data_buffer_toPublisher.resize(SilKitHeaderSize + frame_size);

        asio::async_read(_socket, asio::buffer(_data_buffer_toPublisher.data() + SilKitHeaderSize, frame_size),
                         [this, frame_size](std::error_code ecPayload, std::size_t nReadPayload) {
            if (ecPayload || nReadPayload != frame_size)
            {
                ShutdownAdapter("Socket read error while reading payload (code=" + std::to_string(ecPayload.value())
                                + ", msg=" + ecPayload.message() + ")");
                return;
            }

            SerializeAndPublish(_data_buffer_toPublisher.data(), nReadPayload);
            DoReceiveFrameFromSocket<debug_activated>();
        });
    });
}

SocketToDatagramPubSubAdapter::SocketToDatagramPubSubAdapter(asio::io_context& io_context,
                                                             SilKit::IParticipant* participant,
                                                             const ParsedPubSubConfig& cfg, Endianness endianness,
                                                             uint8_t headerSize)
    : SocketToPubSubAdapter(io_context, participant, cfg.publisherName, cfg.pubSpec, cfg.subscriberName, cfg.subSpec)
    , _headerSize{headerSize}
    , _endianness{endianness}
{
    try
    {
        asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(cfg.address, cfg.port);

        asio::ip::tcp::socket temp_socket(io_context);
        asio::connect(temp_socket, endpoints);
        _socket = asio::generic::stream_protocol::socket(std::move(temp_socket));
    }
    catch (std::exception& e)
    {
        std::ostringstream error_message;
        error_message << e.what() << std::endl;
        error_message << "Error encountered while trying to connect to socket at \"" << cfg.address << ':' << cfg.port
                      << '"';
        throw std::runtime_error(error_message.str());
    }
    _logger->Info("Socket connection successfully established");
    OnSocketConnected();
}

std::unique_ptr<SocketToDatagramPubSubAdapter> SocketToDatagramPubSubAdapter::parseArgument(
    char* datagramSocketTransmitterArg, std::set<std::string>& alreadyProvidedSockets,
    const std::string& participantName, asio::io_context& ioContext, SilKit::IParticipant* participant,
    Endianness endianness, uint8_t headerSize, SilKit::Services::Logging::ILogger* logger)
{
    auto cfg = SocketToPubSubAdapter::parseArgument(datagramSocketTransmitterArg, alreadyProvidedSockets,
                                                    participantName, logger, false);

    std::string debugPrefix = "Created Datagram-PubSub transmitter " + cfg.address + ':' + cfg.port;

    auto newAdapter =
        std::make_unique<SocketToDatagramPubSubAdapter>(ioContext, participant, cfg, endianness, headerSize);

    std::string debug_message = debugPrefix + " <" + cfg.subscriberName + '(' + cfg.subSpec.Topic() + ')' + " >"
                                + cfg.publisherName + '(' + cfg.pubSpec.Topic() + ')';
    logger->Debug(debug_message);
    return newAdapter;
}

void SocketToDatagramPubSubAdapter::WriteOutbound(const uint8_t* data, size_t size)
{
    if (!IsSocketConnected())
    {
        if (_logger && _logger->GetLogLevel() <= SilKit::Services::Logging::Level::Debug)
            _logger->Debug("SocketToDatagramPubSubAdapter: outbound dropped (socket not connected)");
        return;
    }
    auto frameSizeBytes =
        adapters::datagram_socket::WriteFrameSizeToArray(static_cast<uint32_t>(size), _endianness, _headerSize);
    asio::write(_socket, asio::buffer(frameSizeBytes.data(), _headerSize));
    asio::write(_socket, asio::buffer(data, size));
}

void SocketToDatagramPubSubAdapter::StartReceivingFromSocket()
{
    if (_debugEnabled)
        DoReceiveFrameFromSocket<true>();
    else
        DoReceiveFrameFromSocket<false>();
}

void SocketToDatagramPubSubAdapter::OnInbound(const SilKit::Services::PubSub::DataMessageEvent& evt)
{
    HandleInboundImpl(_data_buffer_toPublisher, evt);
}