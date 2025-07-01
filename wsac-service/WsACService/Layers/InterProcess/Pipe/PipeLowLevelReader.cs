using System.IO.Pipes;
using WsACService.Layers.InterProcess.Abstractions;
using WsACService.Memory;
using WsACService.Net;

namespace WsACService.Layers.InterProcess.Pipe;

public class PipeLowLevelReader(PipeStream pipe) : ILowLevelReader
{
    private readonly MemoryBuffer _buffer = new();

    public async Task ReadAsync(MemoryBuffer buffer, long size, CancellationToken ct)
    {
        buffer.Reserve(size);
        await pipe.ReadAsync(buffer, size, ct);
    }

    public void Read(Span<byte> buffer, CancellationToken ct)
    {
        pipe.Read(buffer, ct);
    }

    public T Read<T>(CancellationToken ct) where T : unmanaged
    {
        return pipe.Read<T>(ct);
    }

    public Task SkipAsync(long size, CancellationToken ct)
    {
        return pipe.SkipAsync(_buffer, size, ct);
    }
}