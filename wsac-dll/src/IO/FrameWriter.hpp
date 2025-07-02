//
// Created by cve on 2025-07-02.
//

#pragma once

#include "Include.hpp"
#include "Model/FrameHeader.hpp"
#include "PipeWriter.hpp"

namespace wsac::io
{

class FrameWriter
{
    PipeWriter &_inner;

  public:
    explicit FrameWriter(PipeWriter &inner);

    void WriteData(const model::Vector<uint8_t> &data) const;
    void Write(const model::FrameHeader &header) const;
    void WriteCheckpoint() const;
    void WriteRegularHeader() const;
};

} // namespace wsac::io