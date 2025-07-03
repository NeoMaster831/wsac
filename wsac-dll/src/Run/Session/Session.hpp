#pragma once

#include "IO/frame/FrameReader.hpp"
#include "IO/frame/FrameWriter.hpp"
#include "Include.hpp"

namespace wsac::run
{

class Session
{
    io::FrameReader _frameReader;
    io::FrameWriter _frameWriter;

    void HandleCheckpointSignal() const;
    static void HandleRegularSignal(size_t dataSize, const std::stop_token &token);
    void HandleInvalidSignal() const;
    void HandleTestSignal(size_t dataSize, const std::stop_token &token) const;

  public:
    explicit Session(io::PipeReader &reader, io::PipeWriter &writer);
    void Run(const std::stop_token &token) const;
};

} // namespace wsac::run
