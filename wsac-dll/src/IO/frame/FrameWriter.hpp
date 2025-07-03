//
// Created by cve on 2025-07-02.
//

#pragma once

#include "IO/pipe/PipeWriter.hpp"
#include "Model/FrameHeader.hpp"

namespace wsac::io
{

class FrameWriter
{
    PipeWriter &_inner;

  public:
    explicit FrameWriter(PipeWriter &inner);

    void WriteData(model::Bytes data) const;
    void Write(const model::FrameHeader &header) const;
    void WriteCheckpoint() const;
    void WriteRegularHeader() const;
};

} // namespace wsac::io