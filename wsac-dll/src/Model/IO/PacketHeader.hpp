//
// Created by cve on 2025-07-03.
//

#pragma once

#include "Include.hpp"

namespace wsac::model
{

struct PacketHeader
{
    std::array<uint8_t, 12> nonce{};
    std::array<uint8_t, 16> mac{};
};

} // namespace wsac::model