namespace WsACService.IO.Abstractions;

public interface ILowLevelReader
{
    public long Available { get; }

    public ILowLevelReader CreateChild(long size);
    
    public Task ReadAsync(Stream stream, long size, CancellationToken ct);
    
    public T Read<T>(CancellationToken ct) where T : unmanaged;

    public Task SkipAsync(long size, CancellationToken ct);
}
