#pragma once

#include "Include.hpp"

namespace wsac::net
{

class MessageReader
{
    HANDLE _pipe;

  public:
    explicit MessageReader(HANDLE pipe);

    bool Read(void *p, DWORD size, const std::stop_token &st) const;
    bool RecoverDueMagicByte(const std::stop_token &st) const;
};

} // namespace wsac::net