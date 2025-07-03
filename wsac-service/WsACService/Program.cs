using WsACService.Logging;
using WsACService.Services;

namespace WsACService;

public static class Program
{
    public static async Task Main()
    {
        var settings = new HostApplicationBuilderSettings
        {
            ApplicationName = "WsACService"
        };
        var builder = Host.CreateEmptyApplicationBuilder(settings);

        builder.Logging
            .ClearProviders()
            .AddConsoleLogger();
        
        builder.Services
            .AddHostedService<PipeSessionService>();

        var host = builder.Build();
        await host.StartAsync();
    }
}