namespace WsACService.IO.Abstractions;

public interface IReader
{
    public long Available { get; }

    public IReader CreateChild(long size);
    
    public Task ReadAsync(Stream stream, long size, CancellationToken ct);
    
    public T Read<T>(CancellationToken ct) where T : unmanaged;

    public Task SkipAsync(long size, CancellationToken ct);
}
