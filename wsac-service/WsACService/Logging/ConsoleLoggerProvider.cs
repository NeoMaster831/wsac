using System.Collections.Concurrent;

namespace WsACService.Logging;

[ProviderAlias("Console")]
public sealed class ConsoleLoggerProvider : ILoggerProvider
{
    private static ConsoleLoggerProvider? _instance;

    public static ConsoleLoggerProvider Instance
    {
        get
        {
            if (_instance is null)
                throw new NullReferenceException("Instance is not initialized yet");
            if (_instance._disposed)
                throw new ObjectDisposedException(nameof(ConsoleLoggerProvider));

            return _instance;
        }
    }

    private readonly ConcurrentDictionary<string, ConsoleLogger> _loggers = new(StringComparer.Ordinal);

    private bool _disposed;

    public ConsoleLoggerProvider()
    {
        if (Interlocked.Exchange(ref _instance, this) is { _disposed: false })
            throw new ApplicationException("Only one instance of ConsoleLoggerProvider is allowed.");
    }

    public ILogger CreateLogger(string categoryName)
    {
        return _loggers.GetOrAdd(categoryName, static name => new ConsoleLogger(name));
    }

    public void Dispose()
    {
        _disposed = true;
        _loggers.Clear();
    }
}