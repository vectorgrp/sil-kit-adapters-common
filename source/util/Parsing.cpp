// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#include "Parsing.hpp"
#include <iostream>
#include <algorithm>
#include <array>
#include <cstring>

const std::string adapters::regUriArg = "--registry-uri";

const std::string adapters::configurationArg = "--configuration";

const std::string adapters::logLevelArg = "--log";

const std::string adapters::participantNameArg = "--name";

const std::string adapters::helpArg = "--help";

char** util::findArg(int argc, char** argv, const std::string& argument, char** args)
{
    auto found = std::find_if(args, argv + argc, [argument](const char* arg) -> bool {
        return argument == arg;
    });
    if (found < argv + argc)
    {
        return found;
    }
    return NULL;
};

char** util::findArgOf(int argc, char** argv, const std::string& argument, char** args)
{
    auto found = findArg(argc, argv, argument, args);
    if (found != NULL && found + 1 < argv + argc)
    {
        return found + 1;
    }
    return NULL;
};

std::string util::getArgDefault(int argc, char** argv, const std::string& argument, const std::string& defaultValue)
{
    auto found = findArgOf(argc, argv, argument, argv);
    if (found != NULL)
        return *(found);
    else
        return defaultValue;
};

