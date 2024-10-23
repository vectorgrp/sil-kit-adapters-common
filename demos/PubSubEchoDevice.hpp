#include <iostream>
#include <string>
#include <thread>

#include "silkit/SilKit.hpp"
#include "silkit/config/all.hpp"
#include "silkit/services/pubsub/all.hpp"
#include "silkit/util/serdes/Serialization.hpp"
#include "util/Parsing.hpp"
#include "adapter/Cli.hpp"

using namespace adapters;
using namespace util;

using namespace SilKit::Services::PubSub;

inline int EchoDemoMain(int argc, char** argv, const std::string participantName, PubSubSpec subDataSpec, PubSubSpec pubDataSpec)
{
    if (findArg(argc, argv, "--help", argv) != nullptr)
    {
        std::cout << "Usage (defaults in curly braces if you omit the switch):" << std::endl
            << argv[0] << " [" << participantNameArg << " <participant's name{EchoDevice}>]\n"
            "  [" << regUriArg << " silkit://<host{localhost}>:<port{8501}>]\n"
            "  [" << logLevelArg << " <Trace|Debug|Warn|{Info}|Error|Critical|Off>]\n";
        return 0;
    }

    const std::string loglevel = getArgDefault(argc, argv, logLevelArg, "Info");
    const std::string registryURI = getArgDefault(argc, argv, regUriArg, "silkit://localhost:8501");

    const std::string participantConfigurationString =
        R"({ "Logging": { "Sinks": [ { "Type": "Stdout", "Level": ")" + loglevel + R"("} ] } })";

    try
    {
        auto participantConfiguration =
            SilKit::Config::ParticipantConfigurationFromString(participantConfigurationString);

        auto participant = SilKit::CreateParticipant(participantConfiguration, participantName, registryURI);

        auto dataPublisher = participant->CreateDataPublisher(participantName + "_pub", pubDataSpec);

        std::string line_buffer;

        auto dataSubscriber = participant->CreateDataSubscriber(
            participantName + "_sub", subDataSpec,
            [&](SilKit::Services::PubSub::IDataSubscriber* subscriber, const DataMessageEvent& dataMessageEvent) {
                if (dataMessageEvent.data.size() <= 4)
                {
                    std::cerr << "warning: message received probably wasn't following SAB format." << std::endl;
                    line_buffer += std::string(reinterpret_cast<const char*>(dataMessageEvent.data.data()),
                        dataMessageEvent.data.size());
                }
                else
                {
                    line_buffer += std::string(reinterpret_cast<const char*>(dataMessageEvent.data.data() + 4),
                        dataMessageEvent.data.size() - 4);
                }
                std::string::size_type newline_pos;
                while ((newline_pos = line_buffer.find_first_of('\n')) != std::string::npos)
                {
                    std::cout << "SIL Kit >> SIL Kit: " << line_buffer.substr(0, newline_pos) << std::endl;
                    line_buffer.erase(0, newline_pos + 1);
                }
                dataPublisher->Publish(dataMessageEvent.data);
            });

        promptForExit();
    }
    catch (const SilKit::ConfigurationError& error)
    {
        std::cerr << "Invalid configuration: " << error.what() << std::endl;
        return CodeErrorConfiguration;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Something went wrong: " << error.what() << std::endl;
        return CodeErrorOther;
    }

    return CodeSuccess;
}
