using System.Buffers;
using System.IO.Pipes;
using WsACService.InterProcess.Abstractions;
using WsACService.IO.Abstractions;
using WsACService.Net;

namespace WsACService.InterProcess.Pipe;

public class PipeLowLevelReader(PipeStream pipe) : ILowLevelReader
{
    public Stream AsStream()
    {
        return pipe;
    }
    
    private async Task ReadAsyncInternal(Stream? stream, long size, CancellationToken ct)
    {
        using var buffer = MemoryPool<byte>.Shared.Rent(8192);

        for (long i = 0; i < size && !ct.IsCancellationRequested;)
        {
            var read = await pipe.ReadAsync(buffer.Memory, ct);
            if (read == 0)
                throw new EndOfStreamException();

            stream?.Write(buffer.Memory.Span[..read]);
            i += read;
        }

        ct.ThrowIfCancellationRequested();
    }

    public Task ReadAsync(Stream? stream, long size, CancellationToken ct)
    {
        return ReadAsyncInternal(stream, size, ct);
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
        return ReadAsyncInternal(null, size, ct);
    }
}
