namespace WsACService.Logging;

public static class LogExtension
{
    public static void Info(this ILogger logger, int id, string message)
    {
        logger.LogInformation("[{id}] {msg}", id, message);
    }
    
    public static void Warn(this ILogger logger, int id, string message)
    {
        logger.LogWarning("[{id}] {msg}", id, message);
    }
    
    public static void Error(this ILogger logger, int id, string message)
    {
        logger.LogError("[{id}] {msg}", id, message);
    }
}
