#include "MessageReader.hpp"

namespace
{

bool DataWaiting(const HANDLE ps, bool *waiting)
{
    DWORD avail;
    if (PeekNamedPipe(ps, nullptr, 0, nullptr, &avail, nullptr))
    {
        *waiting = avail > 0;
        return true;
    }

    return false;
}

} // namespace

namespace wsac::net
{

MessageReader::MessageReader(const HANDLE pipe) : _pipe(pipe)
{
}

bool MessageReader::Read(void *p, const DWORD size, const std::stop_token &st) const
{
    for (DWORD i = 0; i < size && !st.stop_requested();)
    {
        bool waiting;
        do
        {
            if (!DataWaiting(_pipe, &waiting))
                return false;
        } while (!waiting && !st.stop_requested());

        if (st.stop_requested())
            return false;

        DWORD read;
        if (!ReadFile(_pipe, static_cast<uint8_t *>(p) + i, size - i, &read, nullptr))
            return false;
        if (read == 0)
            return false;

        i += read;
    }

    return true;
}

// It has some overheads, I don't care, because it is very unusual. Fuck you attackers.
bool MessageReader::RecoverDueMagicByte(const std::stop_token &st) const
{
    DWORD magic = STOP_MAGIC_BYTE;
    const auto magicPtr = reinterpret_cast<unsigned char *>(&magic);

    size_t combo = 0;
    while (!st.stop_requested() && combo < sizeof magic)
    {
        unsigned char b;
        if (!Read(&b, sizeof b, st))
        {
            return false;
        }

        if (b == magicPtr[combo])
        {
            combo++;
        }
        else
        {
            if (b == magicPtr[0])
                combo = 1;
            else
                combo = 0;
        }
    }

    return true;
}

} // namespace wsac::net