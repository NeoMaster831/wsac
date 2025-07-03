namespace WsACService.IO.Abstractions;

public interface ITypedWriter<in T> : IWriter
{
    public void Write(T value);
}