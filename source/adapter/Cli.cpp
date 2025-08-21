// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "Cli.hpp"

#include <future>
#include <iostream>

#include "SignalHandler.hpp"

void adapters::promptForExit()
{
    std::promise<int> signalPromise;
    auto signalValue = signalPromise.get_future();
    adapters::RegisterSignalHandler([&signalPromise](auto sigNum) { signalPromise.set_value(sigNum); });

    std::cout << "Press CTRL + C to stop the process..." << std::endl;

    signalValue.wait();

    std::cout << "\nSignal " << signalValue.get() << " received!" << std::endl;
    std::cout << "Exiting..." << std::endl;
}