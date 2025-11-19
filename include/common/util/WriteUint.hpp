// SPDX-FileCopyrightText: Copyright 2025 Vector Informatik GmbH
// SPDX-License-Identifier: MIT

#pragma once

// Restored WriteUint helpers for frame size serialization used by adapters.
#include <type_traits>
#include <cstddef>
#include <asio/ts/buffer.hpp>

#include "Exceptions.hpp"

namespace adapters {
namespace datagram_socket {

enum class Endianness
{
    big_endian,
    little_endian
};

inline std::array<uint8_t, 4> WriteFrameSizeToArray(const uint32_t value, const Endianness endianness,
                                                    const uint8_t headerSize)
{
    if (headerSize < 1 || headerSize > 4)
        throw std::runtime_error("Header size must be between 1 and 4 bytes");

    // Compute max encodable value for given headerSize
    const uint32_t maxVal =
        (headerSize == 4) ? std::numeric_limits<uint32_t>::max() : ((uint32_t{1} << (headerSize * 8)) - 1);
    if (value > maxVal)
        throw std::runtime_error("Frame size exceeds encodable range for chosen header size");

    std::array<uint8_t, 4> result = {};
    for (uint8_t i = 0; i < headerSize; ++i)
    {
        uint8_t byteValue = static_cast<uint8_t>(value >> (8 * i));
        const uint8_t targetIndex =
            (endianness == Endianness::little_endian) ? i : static_cast<uint8_t>(headerSize - 1 - i);
        result[targetIndex] = byteValue;
    }
    return result;
}
} // namespace datagram_socket
} // namespace adapters
