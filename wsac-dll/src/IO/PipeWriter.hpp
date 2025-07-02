#pragma once

#include "../Model/Vector.hpp"
#include "Include.hpp"

namespace wsac::io
{
class PipeWriter
{
    HANDLE _pipe;

  public:
    explicit PipeWriter(void *pipe);
    void Write(const model::Vector<uint8_t> &v) const;
};

} // namespace wsac::io