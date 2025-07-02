//
// Created by cve on 2025-07-02.
//

#include "PipeWriter.hpp"

namespace wsac::io
{

PipeWriter::PipeWriter(void *pipe) : _pipe(pipe)
{
}
void PipeWriter::Write(const model::Vector<uint8_t> &v) const
{
    for (DWORD i = 0; i < v.size();)
    {
        DWORD written;
        if (!WriteFile(_pipe, v.data() + i, v.size() - i, &written, nullptr))
            throw PipeWriteException();

        i += written;
    }
}

} // namespace wsac::io