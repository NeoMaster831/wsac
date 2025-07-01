using System.Diagnostics;
using System.IO.Pipes;
using WsACService.Net;

namespace WsACService.Memory;

public static class MemoryBufferExtension
{
    public static async Task SkipAsync(this Stream stream, MemoryBuffer buffer, long size, CancellationToken ct)
    {
        Debug.Assert(size >= 0);
        Debug.Assert(buffer.Length > 0);

        if (size == 0)
            return;

        var bufferSize = (int)Math.Min(Math.Min(buffer.Length, size), int.MaxValue);
        for (long i = 0; i < size && !ct.IsCancellationRequested;)
        {
            ct.ThrowIfCancellationRequested();

            var read = await stream.ReadAsync(buffer.Memory[..bufferSize], ct);
            if (read == 0)
                throw new EndOfStreamException();

            i += read;
        }
    }

    public static async Task ReadAsync(this PipeStream stream, MemoryBuffer buffer, long size, CancellationToken ct)
    {
        if (size <= int.MaxValue)
        {
            await stream.ReadExactlyAsync(buffer.Memory[..(int)size], ct);
            return;
        }
        
        // Unavoidable synchronous IO operation : BCL only supports int on stream IO
        for (long i = 0; i < size && !ct.IsCancellationRequested;)
        {
            var read = (int)Math.Min(int.MaxValue, size - i);
            stream.Read(buffer.Slice(i, read).Span, ct);
            i += read;
        }
    }

    public static async Task WriteAsync(this PipeStream stream, MemoryBuffer buffer, long size, CancellationToken ct)
    {
        if (size <= int.MaxValue)
        {
            await stream.WriteAsync(buffer.Memory, ct);
            return;
        }
        
        // Unavoidable synchronous IO operation : BCL only supports int on stream IO
        for (long i = 0; i < size && !ct.IsCancellationRequested;)
        {
            var written = (int)Math.Min(int.MaxValue, size - i);
            stream.Write(buffer.Slice(i, written).Span);
            i += written;
        }
    }
}