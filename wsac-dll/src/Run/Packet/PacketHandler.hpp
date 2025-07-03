//
// Created by cve on 2025-07-03.

#pragma once

#include "Include.hpp"

namespace wsac::run
{
/**
 * Packet handler singleton.
 */
class PacketHandler
{

  public:
    explicit PacketHandler(std::stop_token);
    ~PacketHandler();
};
} // namespace wsac::run