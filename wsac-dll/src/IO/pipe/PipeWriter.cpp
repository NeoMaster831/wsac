//
// Created by cve on 2025-07-02.
//

#include "PipeWriter.hpp"

#include "Log.hpp"

namespace wsac::io
{

PipeWriter::PipeWriter(void *pipe) : _pipe(pipe)
{
}
void PipeWriter::Write(const model::ConstBytes v) const
{
    LogVbLn("writing %zu bytes to pipe...", v.size());
    for (DWORD i = 0; i < v.size();)
    {
        DWORD written;

        if (!WriteFile(_pipe, static_cast<const uint8_t *>(v.data()) + i, v.size() - i, &written, nullptr))
            throw PipeWriteException();
        i += written;
    }
    LogVbLn("done writing");
}

} // namespace wsac::io