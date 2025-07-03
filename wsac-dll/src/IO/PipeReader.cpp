//
// Created by cve on 2025-07-02.
//

#include "PipeReader.hpp"

namespace wsac::io
{

PipeReader::PipeReader(void *pipe) : _pipe(pipe)
{
}

void PipeReader::Read(model::Bytes v, const std::stop_token &st) const
{
    for (DWORD i = 0; i < v.size() && !st.stop_requested();)
    {
        bool waiting;
        do
        {
            DWORD avail;
            if (PeekNamedPipe(_pipe, nullptr, 0, nullptr, &avail, nullptr))
                waiting = avail > 0;
            else
                throw PipeReadException();
        } while (!waiting && !st.stop_requested());

        if (st.stop_requested())
            throw StopTokenRequestedException();

        DWORD read;
        if (!ReadFile(_pipe, static_cast<uint8_t*>(v.data()) + i, v.size() - i, &read, nullptr))
            throw PipeReadException();
        if (read == 0)
            throw PipeReadException();

        i += read;
    }

    if (st.stop_requested())
        throw StopTokenRequestedException();
}

}