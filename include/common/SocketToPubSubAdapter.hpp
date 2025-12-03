// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <string>
#include <set>
#include <type_traits>

#include "silkit/SilKit.hpp"
#include "silkit/services/pubsub/all.hpp"
#include "silkit/services/logging/all.hpp"
#include "silkit/util/serdes/Serializer.hpp"
#include "silkit/util/serdes/Deserializer.hpp"

#include "asio/io_context.hpp"
#include "asio/generic/stream_protocol.hpp"
#include <csignal>

namespace adapters {

class SocketToPubSubAdapter
{
protected:
    using PubSubSpec = SilKit::Services::PubSub::PubSubSpec;
    static constexpr std::size_t SilKitHeaderSize = sizeof(uint32_t);
    struct ParsedPubSubConfig
    {
        std::string address;
        std::string port;
        std::string publisherName;
        std::string subscriberName;
        PubSubSpec pubSpec;
        PubSubSpec subSpec;
    };

    SocketToPubSubAdapter(asio::io_context& ioContext, SilKit::IParticipant* participant,
                          const std::string& publisherName, const PubSubSpec& pubSpec,
                          const std::string& subscriberName, const PubSubSpec& subSpec)
        : _ioContext{&ioContext}
        , _socket{ioContext}
        , _logger{participant->GetLogger()}
        , _participant{participant}
        , _publisherName{publisherName}
        , _pubSpec{pubSpec}
        , _subscriberName{subscriberName}
        , _subSpec{subSpec}
        , _publisher{nullptr}
        , _subscriber{nullptr}
        , _debugEnabled{_logger->GetLogLevel() <= SilKit::Services::Logging::Level::Debug}
    {
    }

    virtual ~SocketToPubSubAdapter() = default;

    virtual void StartReceivingFromSocket() = 0;
    virtual void WriteOutbound(const uint8_t* data, size_t size) = 0;
    virtual void OnInbound(const SilKit::Services::PubSub::DataMessageEvent& evt) = 0;

protected:
    void CreatePublisher();
    void CreateSubscriber();
    void OnSocketConnected();
    void SerializeAndPublish(uint8_t* bufferBegin, std::size_t payloadSize);
    void ShutdownAdapter(const std::string& logMessage);
    static ParsedPubSubConfig parseArgument(char* rawArg, std::set<std::string>& alreadyProvidedSockets,
                                            const std::string& participantName,
                                            SilKit::Services::Logging::ILogger* logger, bool isUnixSocket);

    void HandleInboundImpl(std::vector<uint8_t>& buffer, const SilKit::Services::PubSub::DataMessageEvent& evt)
    {
        _deserializer.Reset(SilKit::Util::ToStdVector(evt.data));
        const auto payloadSize = _deserializer.BeginArray();

        buffer.resize(payloadSize);
        for (auto& b : buffer)
        {
            b = _deserializer.Deserialize<uint8_t>(8);
        }
        if (_debugEnabled)
        {
            _logger->Debug("SIL Kit >> Adapter payload size: " + std::to_string(buffer.size()) + " bytes");
        }
        WriteOutbound(buffer.data(), buffer.size());
    }

    template<size_t N>
    void HandleInboundImpl(std::array<uint8_t, N>& buffer, const SilKit::Services::PubSub::DataMessageEvent& evt)
    {
        _deserializer.Reset(SilKit::Util::ToStdVector(evt.data));
        const auto payloadSize = _deserializer.BeginArray();

        if (payloadSize > buffer.size())
        {
            _logger->Error("Inbound payload (" + std::to_string(payloadSize) + ") exceeds fixed buffer capacity ("
                            + std::to_string(buffer.size()) + ")");
            return;
        }
        for (size_t i = 0; i < payloadSize; ++i)
        {
            buffer[i] = _deserializer.Deserialize<uint8_t>(8);
        }
        if (_debugEnabled)
        {
            _logger->Debug("SIL Kit >> Adapter payload size: " + std::to_string(payloadSize) + " bytes");
        }
        WriteOutbound(buffer.data(), payloadSize);
    }

protected:
    asio::io_context* _ioContext;
    asio::generic::stream_protocol::socket _socket;
    SilKit::Services::Logging::ILogger* _logger;
    SilKit::IParticipant* _participant;
    std::string _publisherName;
    PubSubSpec _pubSpec;
    std::string _subscriberName;
    PubSubSpec _subSpec;
    SilKit::Services::PubSub::IDataPublisher* _publisher;
    SilKit::Services::PubSub::IDataSubscriber* _subscriber;
    SilKit::Util::SerDes::Serializer _serializer;
    SilKit::Util::SerDes::Deserializer _deserializer;
    bool _socketConnected{false};
    void MarkSocketConnected()
    {
        _socketConnected = true;
    }
    bool IsSocketConnected() const
    {
        return _socketConnected;
    }
    bool _debugEnabled{false};
};

} // namespace adapters
