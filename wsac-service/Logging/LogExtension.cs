namespace WsACService.Logging;

public static class LogExtension
{
    public static void Info(this ILoggable loggable, string message)
    {
        loggable.Logger.LogInformation("[PipeServer {I}] {Message}", loggable.Identifier, message);
    }

    public static void Error(this ILoggable loggable, string message, Exception e)
    {
        loggable.Logger.LogError(e, "[PipeServer {I}] {Message}", loggable.Identifier, message);
    }
}