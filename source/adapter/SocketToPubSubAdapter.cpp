// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "SocketToPubSubAdapter.hpp"

#include "Parsing.hpp"
#include "StringUtils.hpp"
#include "common/util/SocketEndpointParsing.hpp"
#include "common/util/PubSubTopicParsing.hpp"
#include "silkit/util/serdes/Serialization.hpp"

using namespace adapters;
using namespace SilKit::Services::PubSub;

SocketToPubSubAdapter::ParsedPubSubConfig SocketToPubSubAdapter::parseArgument(
    char* rawArg,
    std::set<std::string>& alreadyProvidedSockets,
    const std::string& participantName,
    SilKit::Services::Logging::ILogger* logger,
    bool isUnixSocket)
{
    auto args = util::split(rawArg, ",");
    auto it = args.begin();

    util::assertAdditionalIterator(it, args);
    throwInvalidCliIf(alreadyProvidedSockets.insert(*it).second == false);

    std::string address, port;
    if (isUnixSocket)
    {
        util_socket::extractUnixSocket(address, port, it);
    }
    else
    {
        util_socket::extractTcpSocket(address, port, it);
    }

    // inbound topic & labels
    util::assertAdditionalIterator(it, args);
    std::string subscriberName;
    std::string subscriberNamespace;
    util_topic::extractAndEraseNamespaceAndDefaultnameFrom(*it, subscriberName, subscriberNamespace);
    PubSubSpec subSpec(*it++, SilKit::Util::SerDes::MediaTypeData());
    if (!subscriberNamespace.empty())
        subSpec.AddLabel("Namespace", subscriberNamespace, SilKit::Services::MatchingLabel::Kind::Mandatory);
    util_topic::extractTopicLabels(args, it, subSpec);

    // outbound topic & labels
    util::assertAdditionalIterator(it, args);
    std::string publisherName;
    std::string publisherNamespace;
    util_topic::extractAndEraseNamespaceAndDefaultnameFrom(*it, publisherName, publisherNamespace);
    PubSubSpec pubSpec(*it++, SilKit::Util::SerDes::MediaTypeData());
    if (!publisherNamespace.empty())
        pubSpec.AddLabel("Namespace", publisherNamespace, SilKit::Services::MatchingLabel::Kind::Optional);
    util_topic::extractTopicLabels(args, it, pubSpec);

    throwInvalidCliIf(it != args.end());

    if (subscriberName.empty())
        subscriberName = util_topic::generateSubscriberNameFrom(participantName);
    if (publisherName.empty())
        publisherName = util_topic::generatePublisherNameFrom(participantName);

    logger->Debug("Parsed pub/sub argument for address=" + address);
    return ParsedPubSubConfig{address, port, publisherName, subscriberName, pubSpec, subSpec};
}

void SocketToPubSubAdapter::CreatePublisher()
{
    if (_publisher) return;
    _publisher = _participant->CreateDataPublisher(_publisherName, _pubSpec);
}

void SocketToPubSubAdapter::CreateSubscriber()
{
    if (_subscriber) return;
    _subscriber = _participant->CreateDataSubscriber(
        _subscriberName, _subSpec,
        SilKit::Services::PubSub::DataMessageHandler{
            [this](SilKit::Services::PubSub::IDataSubscriber*, const SilKit::Services::PubSub::DataMessageEvent& evt) {
                OnInbound(evt);
            }});
}

void SocketToPubSubAdapter::OnSocketConnected()
{
    MarkSocketConnected();
    CreatePublisher();
    CreateSubscriber();
    StartReceivingFromSocket();
}

void SocketToPubSubAdapter::SerializeAndPublish(uint8_t* bufferBegin, std::size_t payloadSize)
{
    _serializer.BeginArray(payloadSize);
    auto serBuf = _serializer.ReleaseBuffer(); // length prefix (SilKitHeaderSize bytes)
    assert(serBuf.size() == SilKitHeaderSize);
    std::memcpy(bufferBegin, serBuf.data(), serBuf.size());
    _publisher->Publish(SilKit::Util::Span<uint8_t>(bufferBegin, serBuf.size() + payloadSize));
}

void SocketToPubSubAdapter::ShutdownAdapter(const std::string& logMessage)
{
    if (_logger)
        _logger->Error(logMessage);
    _socket.close();
    if (_ioContext)
        _ioContext->stop();
#ifdef WIN32
    GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
#else
    std::raise(SIGINT);
#endif
}