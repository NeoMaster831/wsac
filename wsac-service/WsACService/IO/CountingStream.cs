namespace WsACService.IO;

public class CountingStream(Stream inner, long length) : Stream
{
    public int ReadSize  { get; private set; }
    public int WriteSize { get; private set; }

    public override void Flush()
    {
        inner.Flush();
    }

    public override int Read(byte[] buffer, int offset, int count)
    {
        var read = inner.Read(buffer, offset, count);
        ReadSize += read;
        return read;
    }

    public override long Seek(long offset, SeekOrigin origin)
    {
        throw new NotSupportedException();
    }

    public override void SetLength(long value)
    {
        throw new NotSupportedException();
    }

    public override void Write(byte[] buffer, int offset, int count)
    {
        inner.Write(buffer, offset, count);
        WriteSize += count;
    }

    public override bool CanRead  => true;
    public override bool CanSeek  => false;
    public override bool CanWrite => true;

    public override long Length => length - ReadSize;

    public override long Position
    {
        get => throw new NotSupportedException();
        set => throw new NotSupportedException();
    }
}