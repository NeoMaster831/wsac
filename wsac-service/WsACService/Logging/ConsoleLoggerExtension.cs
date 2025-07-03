using Microsoft.Extensions.Logging.Configuration;

namespace WsACService.Logging;

public static class ConsoleLoggerExtension
{
    public static ILoggingBuilder AddConsoleLogger(this ILoggingBuilder builder)
    {
        builder.AddConfiguration();
        builder.Services.AddSingleton<ILoggerProvider, ConsoleLoggerProvider>();
        
        return builder;
    }
}