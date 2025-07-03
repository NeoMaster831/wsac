//
// Created by cve on 2025-07-03.
//

#pragma once
#include "IO/pipe/PipeReader.hpp"

namespace wsac::io
{

class PacketReader
{

  public:
    explicit PacketReader(PipeReader &reader);
};

} // namespace wsac::io
