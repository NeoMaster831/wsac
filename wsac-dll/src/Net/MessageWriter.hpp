#pragma once

#include "MessageReader.hpp"

namespace wsac::net
{

class MessageWriter
{
    HANDLE _pipe;

  public:
    explicit MessageWriter(HANDLE pipe);

    bool Write(void *p, DWORD size) const;
};

} // namespace wsac::net