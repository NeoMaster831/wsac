//
// Created by cve on 2025-07-03.
//

#pragma once
#include "IO/frame/FrameWriter.hpp"
#include "Model/IO/PacketHeader.hpp"

namespace wsac::io
{

class PacketWriter
{
    FrameWriter &_inner;

  public:
    explicit PacketWriter(FrameWriter &inner);

    void WriteData(model::Bytes data) const;
    void WritePrevious(model::PacketHeader &header) const;
};
} // namespace wsac::io