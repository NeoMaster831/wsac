using WsACService.IO.Abstractions;
using WsACService.Net.Models;

namespace WsACService.IO;

public readonly struct FrameWriter(IWriter inner) : ITypedWriter<FrameHeader>
{
    public void Write(FrameHeader header)
    {
        inner.Write(Config.Preamble);
        inner.Write(header);
    }
    
    void IWriter.Write<T>(T value)
    {
        inner.Write(value);
    }

    public Task WriteAsync(Stream stream, CancellationToken ct)
    {
        return inner.WriteAsync(stream, ct);
    }
}
