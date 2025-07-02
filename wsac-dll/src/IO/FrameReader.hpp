#pragma once

#include "Include.hpp"
#include "Model/FrameHeader.hpp"
#include "PipeReader.hpp"

namespace wsac::io
{

class FrameReader
{

    PipeReader &_inner;

  public:
    explicit FrameReader(PipeReader &inner);

    void ReadUntilPreamble(const std::stop_token& st) const;
    model::FrameHeader ReadHeader(const std::stop_token &st) const;
};

} // namespace wsac::io
