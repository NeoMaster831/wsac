using System.Buffers;
using System.Diagnostics;
using WsACService.Logging;

namespace WsACService.Memory;

public struct BufferSegment
{
    public MemoryBuffer Buffer { get; }
    public long         Offset { get; }
    public int          Length { get; }

    public BufferSegment(MemoryBuffer buffer, long offset, int length)
    {
        Buffer = buffer;
        Offset = offset;
        Length = length;
    }

    public Span<byte> Span
    {
        get
        {
            if (Buffer.Length < int.MaxValue) 
                return Buffer.Span;
            
            unsafe
            {
                fixed (byte* ptr = Buffer.Buffer)
                {
                    return new Span<byte>(ptr + Offset, Length);
                }
            }
        }
    }
}

public class MemoryBuffer
{
    public byte[] Buffer { get; private set; }

    public MemoryBuffer(int size = 1024)
    {
        Debug.Assert(size > 0);

        Buffer = ArrayPool<byte>.Shared.Rent(size);
    }

    public void Reserve(long size)
    {
        Debug.Assert(size > 0);

        if (Buffer.LongLength >= size)
            return;

        if (size > int.MaxValue)
        {
            if (Buffer.LongLength <= int.MaxValue)
                ArrayPool<byte>.Shared.Return(Buffer);

            var mib = size / 1024 / 1024;
            GlobalLogger.Shared.Warn(-1, $"LOH allocation ({mib} MiB)");

            // WARNING: LOH allocation (> 2GiB)
            Buffer = new byte[size];
        }
        else
        {
            ArrayPool<byte>.Shared.Return(Buffer);
            Buffer = ArrayPool<byte>.Shared.Rent((int)size);
        }
    }

    public long Length => Buffer.LongLength;

    public BufferSegment Slice(long offset, int size)
    {
        return new BufferSegment(this, offset, size);
    }

    public Memory<byte> Memory => Buffer;

    public Span<byte> Span => Buffer.AsSpan();
}