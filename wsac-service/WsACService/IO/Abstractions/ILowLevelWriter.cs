namespace WsACService.IO.Abstractions;

public interface ILowLevelWriter
{
    public void Write(Span<byte> buffer);

    public void Write<T>(T value) where T : unmanaged;

    public Task WriteAsync(Stream stream, CancellationToken ct);
}
