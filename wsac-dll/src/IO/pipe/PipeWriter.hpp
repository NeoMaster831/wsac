#pragma once

#include "Include.hpp"
#include "Model/Bytes.hpp"

namespace wsac::io
{
class PipeWriter
{
    HANDLE _pipe;

  public:
    explicit PipeWriter(void *pipe);
    void Write(model::ConstBytes v) const;
};

} // namespace wsac::io