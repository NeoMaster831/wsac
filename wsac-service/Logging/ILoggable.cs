namespace WsACService.Logging;

public interface ILoggable
{
    public int Identifier { get; }
    public ILogger Logger { get; }
}
