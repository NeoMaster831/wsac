using System.Buffers;
using System.ComponentModel;
using System.IO.Pipes;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

namespace WsACService.Net;

public static partial class PipeStreamExtension
{
    [LibraryImport("kernel32.dll", EntryPoint = "PeekNamedPipe", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static partial bool PeekNamedPipe(
        SafePipeHandle hPipe,
        IntPtr lpBuffer,
        uint nBufferSize,
        IntPtr lpBytesRead,
        out uint lpTotalBytesAvail,
        IntPtr lpBytesLeftThisMessage);

    private static bool DataWaiting(this PipeStream ps)
    {
        if (!ps.IsConnected) return false;
        if (PeekNamedPipe(ps.SafePipeHandle,
                IntPtr.Zero, 0, IntPtr.Zero,
                out var available, IntPtr.Zero)) return available > 0;
        var error = Marshal.GetLastWin32Error();
        if (error == 109)
            throw new EndOfStreamException();
        throw new Win32Exception(Marshal.GetLastWin32Error());
    }

    public static void Read(this PipeStream stream, Span<byte> buffer, CancellationToken ct)
    {
        const int initialSleep = 10;
        const int maxSleep = 100 * 1000;

        var sleep = initialSleep;
        for (var i = 0; i < buffer.Length && !ct.IsCancellationRequested;)
        {
            while (!DataWaiting(stream) && !ct.IsCancellationRequested)
            {
                Thread.Sleep(TimeSpan.FromMicroseconds(sleep));
                sleep *= 2;
                if (sleep > maxSleep)
                    sleep = maxSleep;
            }

            sleep = initialSleep;

            if (ct.IsCancellationRequested)
                break;

            var j = stream.Read(buffer[i..]);
            if (j == 0)
                throw new EndOfStreamException();
            i += j;
        }

        ct.ThrowIfCancellationRequested();
    }

    public static T Read<T>(this PipeStream stream, CancellationToken ct) where T : unmanaged
    {
        unsafe
        {
            Span<byte> buffer = stackalloc byte[sizeof(T)];
            stream.Read(buffer, ct);
            return MemoryMarshal.Read<T>(buffer);
        }
    }

    public static IMemoryOwner<byte> Read(this PipeStream stream, int size, CancellationToken ct)
    {
        var buffer = MemoryPool<byte>.Shared.Rent(size);
        Read(stream, buffer.Memory.Span, ct);
        return buffer;
    }
}