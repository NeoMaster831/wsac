//
// Created by cve on 2025-07-03.
//

#include "PacketWriter.hpp"

namespace wsac::io
{

PacketWriter::PacketWriter(FrameWriter &inner) : _inner(inner)
{
}
void PacketWriter::WriteData(const model::Bytes data) const
{
    _inner.WriteData(data);
}
void PacketWriter::WritePrevious(model::PacketHeader &header) const
{
    _inner.WriteRegularHeader();
    _inner.WriteData(&header);
}
} // namespace wsac::io