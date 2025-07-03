using WsACService.Logging;

namespace WsACService.Net;

public class SessionState(int id)
{
    public int Id { get; } = id;
    
    public FrameSessionState FrameSessionState { get; set; }
    public PacketSessionState PacketSessionState { get; set; }

    public ILogger CreateLogger(string name)
    {
        return ConsoleLoggerProvider.Instance.CreateLogger($"{name}:{Id}");
    }
}
