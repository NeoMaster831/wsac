using WsACService.Memory;

namespace WsACService.InterProcess.Abstractions;

public interface ILowLevelReader
{
    public Task ReadAsync(MemoryBuffer buffer, long size, CancellationToken ct);

    public void Read(Span<byte> buffer, CancellationToken ct);
    
    public T Read<T>(CancellationToken ct) where T : unmanaged;

    public Task SkipAsync(long size, CancellationToken ct);
}
