//
// Created by cve on 2025-07-02.
//

#include "FrameWriter.hpp"

#include "Log.hpp"

namespace wsac::io
{

FrameWriter::FrameWriter(PipeWriter &inner) : _inner(inner)
{
}
void FrameWriter::WriteData(const model::Bytes data) const
{
    _inner.Write(data);
}
void FrameWriter::Write(const model::FrameHeader &header) const
{
    _inner.Write(PREAMBLE);
    _inner.Write(&header);
}
void FrameWriter::WriteCheckpoint() const
{
    const model::FrameHeader header(model::FrameSig::Checkpoint, 0);
    Write(header);
}
void FrameWriter::WriteRegularHeader() const
{
    const model::FrameHeader header(model::FrameSig::Regular, 0);
    Write(header);
}
void FrameWriter::WriteTestData(const model::Bytes data) const
{
    const model::FrameHeader header(model::FrameSig::Test, data.size());
    Write(header);
    WriteData(data);
}

} // namespace wsac::io