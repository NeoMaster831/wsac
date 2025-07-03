namespace WsACService.Logging;

public class ConsoleLogger(string name) : ILogger
{
    private Dictionary<LogLevel, ConsoleColor> ColorMap { get; set; } = new()
    {
        [LogLevel.Trace]       = ConsoleColor.Gray,
        [LogLevel.Debug]       = ConsoleColor.Cyan,
        [LogLevel.Information] = ConsoleColor.Green,
        [LogLevel.Warning]     = ConsoleColor.Yellow,
        [LogLevel.Error]       = ConsoleColor.Red,
        [LogLevel.Critical]    = ConsoleColor.Magenta,
    };
    
    public void Log<TState>(
        LogLevel                         logLevel,
        EventId                          eventId,
        TState                           state,
        Exception?                       exception,
        Func<TState, Exception?, string> formatter)
    {
        if (!IsEnabled(logLevel))
            return;

        var old = Console.ForegroundColor;
        Console.ForegroundColor = ColorMap[logLevel];

        Console.Write($"[{eventId.Id,2}: {logLevel,-12}]    {name} - {formatter(state, exception)}");

        Console.ForegroundColor = old;
        Console.WriteLine();
    }

    public bool IsEnabled(LogLevel _) => true;

    public IDisposable? BeginScope<TState>(TState state) where TState : notnull => null;
}
