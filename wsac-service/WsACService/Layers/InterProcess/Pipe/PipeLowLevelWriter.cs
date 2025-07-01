using System.IO.Pipes;
using WsACService.Layers.InterProcess.Abstractions;
using WsACService.Memory;

namespace WsACService.Layers.InterProcess.Pipe;

public class PipeLowLevelWriter(PipeStream pipe) : ILowLevelWriter
{
    public void Write(Span<byte> buffer)
    {
        pipe.Write(buffer);
    }

    public void Write<T>(T value) where T : unmanaged
    {
        unsafe
        {
            Span<byte> buffer = stackalloc byte[sizeof(T)];
            pipe.Write(buffer);
        }
    }

    public Task WriteAsync(MemoryBuffer buffer, long size, CancellationToken ct)
    {
        return pipe.WriteAsync(buffer, size, ct);
    }
}