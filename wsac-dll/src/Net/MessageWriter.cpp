#include "MessageWriter.hpp"

namespace wsac::net
{

MessageWriter::MessageWriter(const HANDLE pipe) : _pipe(pipe)
{
}

bool MessageWriter::Write(void *p, const DWORD size) const
{
    for (DWORD i = 0; i < size;)
    {
        DWORD written;
        if (!WriteFile(_pipe, static_cast<uint8_t *>(p) + i, size - i, &written, nullptr))
            return false;

        i += written;
    }

    return true;
}

} // namespace wsac::net