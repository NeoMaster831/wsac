using System.Buffers;
using System.ComponentModel;
using System.IO.Pipes;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;

namespace WsACService.IO;

public static partial class PipeStreamExtension
{
    [LibraryImport("kernel32.dll", EntryPoint = "PeekNamedPipe", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static partial bool PeekNamedPipe(
        SafePipeHandle hPipe,
        IntPtr         lpBuffer,
        uint           nBufferSize,
        IntPtr         lpBytesRead,
        out uint       lpTotalBytesAvail,
        IntPtr         lpBytesLeftThisMessage);

    private static bool DataWaiting(this PipeStream ps)
    {
        if (!ps.IsConnected) return false;
        if (PeekNamedPipe(
                ps.SafePipeHandle,
                IntPtr.Zero,
                0,
                IntPtr.Zero,
                out var available,
                IntPtr.Zero))
            return available > 0;
        var error = Marshal.GetLastWin32Error();
        if (error == 109)
            throw new EndOfStreamException();
        throw new Win32Exception(Marshal.GetLastWin32Error());
    }

    private static void Read(this PipeStream stream, Span<byte> buffer, CancellationToken ct)
    {
        for (var i = 0; i < buffer.Length && !ct.IsCancellationRequested;)
        {
            while (!DataWaiting(stream) && !ct.IsCancellationRequested)
                Thread.Yield();

            if (ct.IsCancellationRequested)
                break;

            var read = stream.Read(buffer[i..]);
            if (read == 0)
                throw new EndOfStreamException();

            i += read;
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
}