using System.Buffers;
using System.IO.Pipes;
using WsACService.IO.Abstractions;

namespace WsACService.IO;

public class PipeLowLevelReader : ILowLevelReader
{
    private readonly PipeStream          _pipe;
    private readonly PipeLowLevelReader? _parent;

    public PipeLowLevelReader(PipeStream pipe)
    {
        _pipe     = pipe;
        Available = long.MaxValue;
    }

    private PipeLowLevelReader(PipeLowLevelReader parent)
    {
        _pipe     = parent._pipe;
        _parent   = parent;
        Available = long.MaxValue;
    }

    public long Available { get; private set; }

    private async Task ReadAsyncInternal(Stream? stream, long size, CancellationToken ct)
    {
        using var buffer = MemoryPool<byte>.Shared.Rent(8192);

        for (long i = 0; i < size && !ct.IsCancellationRequested;)
        {
            var read = await _pipe.ReadAsync(buffer.Memory, ct);
            if (read == 0)
                throw new EndOfStreamException();

            stream?.Write(buffer.Memory.Span[..read]);
            i += read;
        }

        ct.ThrowIfCancellationRequested();
    }

    private void ShrinkAvailable(long size)
    {
        _parent?.ShrinkAvailable(size);

        Available -= size;
        if (Available < 0)
            throw new ApplicationException("Read operation exceeds available bytes count");
    }

    public ILowLevelReader CreateChild(long size)
    {
        if (Available < size)
            throw new ArgumentOutOfRangeException(nameof(size), "CreateChild operation exceeds available bytes count");

        var t = new PipeLowLevelReader(this)
        {
            Available = size
        };
        return t;
    }

    public Task ReadAsync(Stream stream, long size, CancellationToken ct)
    {
        ShrinkAvailable(size);
        return ReadAsyncInternal(stream, size, ct);
    }

    public T Read<T>(CancellationToken ct) where T : unmanaged
    {
        unsafe
        {
            ShrinkAvailable(sizeof(T));
        }

        return _pipe.Read<T>(ct);
    }

    public Task SkipAsync(long size, CancellationToken ct)
    {
        ShrinkAvailable(size);
        return ReadAsyncInternal(null, size, ct);
    }
}