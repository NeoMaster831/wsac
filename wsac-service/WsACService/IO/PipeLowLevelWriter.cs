using System.IO.Pipes;
using WsACService.IO.Abstractions;

namespace WsACService.IO;

public class PipeLowLevelWriter(PipeStream pipe) : ILowLevelWriter
{
    public void Write<T>(T value) where T : unmanaged
    {
        unsafe
        {
            Span<byte> buffer = stackalloc byte[sizeof(T)];
            pipe.Write(buffer);
        }
    }

    public Task WriteAsync(Stream stream, CancellationToken ct)
    {
        return stream.CopyToAsync(pipe, ct);
    }
}
