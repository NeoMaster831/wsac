using WsACService.Memory;

namespace WsACService.InterProcess.Abstractions;

public interface ILowLevelWriter
{
    public void Write(Span<byte> buffer);
    public void Write<T>(T              value) where T : unmanaged;
    public Task WriteAsync(MemoryBuffer buffer, long size, CancellationToken ct);
}