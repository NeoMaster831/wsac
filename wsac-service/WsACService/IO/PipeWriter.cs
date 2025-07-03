using System.IO.Pipes;
using System.Runtime.InteropServices;
using WsACService.IO.Abstractions;

namespace WsACService.IO;

public class PipeWriter(PipeStream pipe) : IWriter
{
    public void Write<T>(T value) where T : unmanaged
    {
        unsafe
        {
            Span<byte> buffer = stackalloc byte[sizeof(T)];
            MemoryMarshal.Write(buffer, in value);
            pipe.Write(buffer);
        }
    }

    public Task WriteAsync(Stream stream, CancellationToken ct)
    {
        return stream.CopyToAsync(pipe, ct);
    }
}
