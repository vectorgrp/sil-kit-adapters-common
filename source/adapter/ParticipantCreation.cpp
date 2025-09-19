// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "ParticipantCreation.hpp"

#include <iostream>
#include <chrono>

#include "asio/io_context.hpp"

#include "Parsing.hpp"

using namespace std::chrono_literals;

using namespace adapters;
using namespace util;

const std::string defaultParticipantName = "SilKitAdapter";

std::unique_ptr<SilKit::IParticipant> adapters::CreateParticipant(
    int argc, char** argv, SilKit::Services::Logging::ILogger*& logger, std::string* participantNameInputOutput,
    SilKit::Services::Orchestration::ILifecycleService** lifecycleService, std::promise<void>* runningStatePromise)
{
    std::unique_ptr<SilKit::IParticipant> participant;

    std::string participantName;
    std::string defaultParticipantName;
    defaultParticipantName =
        ((participantNameInputOutput && !participantNameInputOutput->empty()) ? *participantNameInputOutput
                                                                              : ::defaultParticipantName);
    /* else, let's use the content of the given participantNameInputOutput as default:*/

    participantName = getArgDefault(argc, argv, participantNameArg, defaultParticipantName);

    const std::string registryURI = getArgDefault(argc, argv, regUriArg, "silkit://localhost:8501");

    const std::string configurationFile = getArgDefault(argc, argv, configurationArg, "");

    std::shared_ptr<SilKit::Config::IParticipantConfiguration> participantConfiguration;
    if (!configurationFile.empty())
    {
        participantConfiguration = SilKit::Config::ParticipantConfigurationFromFile(configurationFile);
        static const auto conflictualArguments = {&logLevelArg,
                                                  /* others are correctly handled by SilKit if one is overwritten.*/};
        for (const auto* conflictualArgument : conflictualArguments)
        {
            if (findArg(argc, argv, *conflictualArgument, argv) != nullptr)
            {
                auto configFileName = configurationFile;
                if (configurationFile.find_last_of("/\\") != std::string::npos)
                {
                    configFileName = configurationFile.substr(configurationFile.find_last_of("/\\") + 1);
                }
                std::cout << "[info] Be aware that argument given with " << *conflictualArgument
                          << " can be overwritten by a different value defined in the given configuration file "
                          << configFileName << std::endl;
            }
        }
    }
    else
    {
        const std::string loglevel = getArgDefault(argc, argv, logLevelArg, "Info");
        const std::string participantConfigurationString =
            R"({ "Logging": { "Sinks": [ { "Type": "Stdout", "Level": ")" + loglevel + R"("} ] } })";
        participantConfiguration = SilKit::Config::ParticipantConfigurationFromString(participantConfigurationString);
    }

    participant = SilKit::CreateParticipant(participantConfiguration, participantName, registryURI);

    logger = participant->GetLogger();

    if (lifecycleService)
    {
        *lifecycleService =
            participant->CreateLifecycleService({SilKit::Services::Orchestration::OperationMode::Autonomous});
    }

    if (runningStatePromise)
    {
        (*lifecycleService)->SetStartingHandler([runningStatePromise]() { runningStatePromise->set_value(); });
    }

    if (participantNameInputOutput)
    {
        *participantNameInputOutput = participantName;
    }
    return participant;
}


void adapters::Stop(asio::io_context& ioContext, std::thread& ioContextThread,
                    SilKit::Services::Logging::ILogger& logger, std::promise<void>* runningStatePromise,
                    SilKit::Services::Orchestration::ILifecycleService* lifecycleService,
                    std::future<SilKit::Services::Orchestration::ParticipantState>* lifecycleServiceStartFuture)
{
    ioContext.stop();
    ioContextThread.join();

    if (runningStatePromise)
    {
        auto runningStateStatus = runningStatePromise->get_future().wait_for(15s);
        if (runningStateStatus != std::future_status::ready)
        {
            logger.Debug(
                "Lifecycle Service Stopping: timed out while checking if the participant is currently running.");
        }
        else
        {
            if (lifecycleService)
            {
                lifecycleService->Stop("Adapter stopped by the user.");
            }

            if (lifecycleServiceStartFuture)
            {
                auto finalState = lifecycleServiceStartFuture->wait_for(15s);
                if (finalState != std::future_status::ready)
                {
                    logger.Debug("Lifecycle service stopping: timed out");
                }
            }
        }
    }
}