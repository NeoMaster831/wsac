#pragma once

#include "IO/pipe/PipeReader.hpp"
#include "Include.hpp"
#include "Model/FrameHeader.hpp"

namespace wsac::io
{

class FrameReader
{

    PipeReader &_inner;

  public:
    explicit FrameReader(PipeReader &inner);

    void ReadUntilPreamble(const std::stop_token& st) const;
    [[nodiscard]] model::FrameHeader ReadHeader(const std::stop_token &st) const;
};

} // namespace wsac::io
