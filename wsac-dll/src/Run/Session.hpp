#pragma once

#include "IO/FrameReader.hpp"
#include "IO/FrameWriter.hpp"
#include "Include.hpp"

namespace wsac::run
{

class Session
{
    io::FrameReader _frameReader;
    io::FrameWriter _frameWriter;

    void HandleCheckpointSignal() const;
    void HandleRegularSignal();
    void HandleInvalidSignal() const;

  public:
    explicit Session(io::PipeReader &reader, io::PipeWriter &writer);
    void Run(const std::stop_token &token);
};

} // namespace wsac::run
