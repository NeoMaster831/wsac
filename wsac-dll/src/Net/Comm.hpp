#pragma once

#include "Include.hpp"
#include "MessageReader.hpp"
#include "MessageWriter.hpp"

namespace wsac::net
{

class CommService
{
    HANDLE _pipe;
    MessageReader _reader;
    MessageWriter _writer;

  public:
    explicit CommService(std::stop_token);
    ~CommService();

    [[nodiscard]]
    const MessageReader &GetReader() const noexcept;

    [[nodiscard]]
    const MessageWriter &GetWriter() const noexcept;
};

} // namespace wsac::net