// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <cstring>
#include <algorithm>
#include "Exceptions.hpp"

namespace util {
namespace detail {
template <class switchesWithArgument_type, class switchesWithoutArguments_type>
bool _thereAreUnknownArguments(int argc, char** argv, switchesWithArgument_type&& switchesWithArgument,
                               switchesWithoutArguments_type&& switchesWithoutArguments);
}

/// <summary>
/// Returns whether or not there are unknown arguments in the provided command line
/// with regards to switchesWithArgument and switchesWithoutArguments.
/// </summary>
/// <returns>
/// true if any argument in argv doesn't match any argument
/// in switchesWithArgument nor switchesWithoutArguments
/// </returns>
template <class switchesWithArgument_type, class switchesWithoutArguments_type>
bool thereAreUnknownArguments(int argc, char** argv, switchesWithArgument_type&& switchesWithArgument,
                              switchesWithoutArguments_type&& switchesWithoutArguments)
{
    return detail::_thereAreUnknownArguments(argc, argv, std::forward<switchesWithArgument_type>(switchesWithArgument),
                                             std::forward<switchesWithoutArguments_type>(switchesWithoutArguments));
}
template <class elementType>
bool thereAreUnknownArguments(int argc, char** argv, std::initializer_list<elementType>&& switchesWithArgument,
                              std::initializer_list<elementType>&& switchesWithoutArguments)
{
    return detail::_thereAreUnknownArguments(
        argc, argv, std::forward<std::initializer_list<elementType> >(switchesWithArgument),
        std::forward<std::initializer_list<elementType> >(switchesWithoutArguments));
}


/// <summary>
/// Searches [argv,argv+argc[ for a string matching argument, starting at args.
/// </summary>
/// <param name="argc">length of the available char**.</param>
/// <param name="argv">start of the available char**.</param>
/// <param name="argument">exemplar to search.</param>
/// <param name="args">starting point of the search.</param>
/// <returns>pointer to the found argument, or NULL otherwise</returns>
char** findArg(int argc, char** argv, const std::string& argument, char** args);

/// <summary>
/// Searches [argv,argv+argc[ for a string following a string matching argument, starting at args.
/// </summary>
/// <param name="argc">length of the available char**.</param>
/// <param name="argv">start of the available char**.</param>
/// <param name="argument">exemplar to search.</param>
/// <param name="args">starting point of the search.</param>
/// <returns>pointer to the next char* found after argument, or NULL otherwise</returns>
char** findArgOf(int argc, char** argv, const std::string& argument, char** args);

inline char** findArg(int argc, char** argv, const std::string& argument)
{
    return findArg(argc, argv, argument, argv);
}

inline char** findArgOf(int argc, char** argv, const std::string& argument)
{
    return findArgOf(argc, argv, argument, argv);
}

/// <summary>
/// Searches [argv,argv+argc[ for a string following a string matching argument.
/// Returns defaultValue if not found
/// </summary>
/// <param name="argc">length of the available char**.</param>
/// <param name="argv">start of the available char**.</param>
/// <param name="argument">exemplar to search.</param>
/// <param name="defaultValue">value returned if argument is not present in [argv,argv+argc[.</param>
/// <returns>string containing the string following argument if argument is present, or defaultValue otherwise.</returns>
std::string getArgDefault(int argc, char** argv, const std::string& argument, const std::string& defaultValue);

/// <summary>
/// Small utility function to quickly check if "it" is not "cont.end()"
/// </summary>
template <typename iterator, typename container>
void assertAdditionalIterator(const iterator& it, const container& cont)
{
    adapters::throwInvalidCliIf(it == cont.end());
}

/// <summary>
/// Searches [argv,argv+argc[ for all strings following a string matching argument, and calls action(those strings)
/// </summary>
/// <typeparam name="Action">lambda or function callable on a char*</typeparam>
/// <param name="argc">number of char**</param>
/// <param name="argv">pointer to first char*</param>
/// <param name="argument">examplar to search</param>
/// <param name="action">Action to call with strings following each exemplar found in [argv,argv+argc[</param>
template <typename Action>
void foreachArgDo(int argc, char** argv, const std::string& argument, const Action& action)
{
    for (char** arg = findArgOf(argc, argv, argument, argv); arg != NULL;
         arg = findArgOf(argc, argv, argument, arg + 1))
    {
        action(*arg);
    }
}

namespace detail {
template <class switchesWithArgument_type, class switchesWithoutArguments_type>
bool _thereAreUnknownArguments(int argc, char** argv, switchesWithArgument_type&& switchesWithArgument,
                               switchesWithoutArguments_type&& switchesWithoutArguments)
{
    //skip the executable calling:
    argc -= 1;
    argv += 1;
    while (argc)
    {
        if (strncmp(*argv, "--", 2) != 0)
            return true;
        using switchesCollectionElemType = decltype(*switchesWithArgument.begin());
        auto is_argv = [argv](switchesCollectionElemType ex) { return *argv == *ex; };
        if (std::find_if(switchesWithArgument.begin(), switchesWithArgument.end(), is_argv)
            != switchesWithArgument.end())
        {
            //switches with argument have an argument to ignore, so skip "2"
            argc -= 2;
            argv += 2;
        }
        else if (std::find_if(switchesWithoutArguments.begin(), switchesWithoutArguments.end(), is_argv)
                 != switchesWithoutArguments.end())
        {
            //switches without argument don't have an argument to ignore, so skip "1"
            argc -= 1;
            argv += 1;
        }
        else
            return true;
    }
    return false;
}
} // namespace detail
} // namespace util

namespace adapters {
/// <summary>
/// string containing the argument preceding the URI to the SIL Kit registry.
/// </summary>
extern const std::string regUriArg;

/// <summary>
/// string containing the argument preceding the path to configuration file.
/// </summary>
extern const std::string configurationArg;

/// <summary>
/// string containing the argument preceding the logging verbosity level.
/// </summary>
extern const std::string logLevelArg;

/// <summary>
/// string containing the argument preceding the participant name.
/// </summary>
extern const std::string participantNameArg;

/// <summary>
/// string containing the argument requesting the help message.
/// </summary>
extern const std::string helpArg;

} // namespace adapters
