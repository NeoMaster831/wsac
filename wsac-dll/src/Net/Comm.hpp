#pragma once

#include "IO/pipe/PipeReader.hpp"
#include "IO/pipe/PipeWriter.hpp"
#include "Include.hpp"

namespace wsac::net
{

class Comm
{
    HANDLE _pipe;
    io::PipeReader _reader;
    io::PipeWriter _writer;

  public:
    explicit Comm(const std::stop_token&);
    ~Comm();

    [[nodiscard]]
    io::PipeReader & GetReader();

    [[nodiscard]]
    io::PipeWriter & GetWriter();
};

} // namespace wsac::net