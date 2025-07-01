namespace WsACService.IO.Abstractions;

public interface ILowLevelReader
{
    public Stream AsStream();
    
    public Task ReadAsync(Stream stream, long size, CancellationToken ct);

    public void Read(Span<byte> buffer, CancellationToken ct);
    
    public T Read<T>(CancellationToken ct) where T : unmanaged;

    public Task SkipAsync(long size, CancellationToken ct);
}
