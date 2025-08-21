// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

#include "silkit/SilKit.hpp"

namespace asio {
class io_context;
}

namespace adapters {

//! \brief CreateParticipant can be used to create a SilKit participant and do a host
// of standard things from the arguments given in command line.
// It will provide the unique pointer as a return value and several elements as output parameters
// - logger : the participant's logger. Provided for convenience as this function
//   may already log messages;
// - participantName : the name, if given in command line. If filled, is used as the default value
//   if the command line provides nothing. Provided for convenience
//   as this is required for the creation of this participant. Note this is will not be updated to
//   the content in the configuration file, if it is present there;
// - lifecycleService : the SilKit's LifecycleService, which is created with Autonomous mode.
//   Can be used to monitor the lifecycle of the simulation, particularly the future it creates
//   when starting the simulation;
// - runningStatePromise : a promise created to be set when the System monitor sets this participant
//   as "Running". To be used for lifecycle checking.

std::unique_ptr<SilKit::IParticipant> CreateParticipant(
    int argc, char** argv, SilKit::Services::Logging::ILogger*& logger, std::string* participantName = nullptr,
    SilKit::Services::Orchestration::ILifecycleService** lifecycleService = nullptr,
    std::promise<void>* runningStatePromise = nullptr);

//! \brief Stop can be used to initiate and check the proper shutdown of asio and SilKit facilities.
//  Stop will try to use the provided elements to check for proper shutdown, and will eventually send
//  debug information if something went wrong.
//  Note that lifecycleServiceStartFuture and lifecycleService are used only if runningStatePromise
//  is also provided as the lifecycleService needs to be checked to have been running first (see above).
void Stop(asio::io_context& ioContext, std::thread& ioContextThread, SilKit::Services::Logging::ILogger& logger,
          std::promise<void>* runningStatePromise = nullptr,
          SilKit::Services::Orchestration::ILifecycleService* lifecycleService = nullptr,
          std::future<SilKit::Services::Orchestration::ParticipantState>* lifecycleServiceStartFuture = nullptr);
} // namespace adapters