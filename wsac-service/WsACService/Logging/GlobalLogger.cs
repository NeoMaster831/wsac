namespace WsACService.Logging;

public class GlobalLogger
{
    public static ILogger Shared { get; private set; }
    
    public GlobalLogger(ILogger logger)
    {
        Shared = logger;
    }
}
