using WsACService;

var builder = Host.CreateApplicationBuilder(args);
builder.Services
    .AddSingleton<PipeManager>()
    .AddHostedService<PipeWorker>()
    .AddWindowsService(options => { options.ServiceName = "WsACService"; });

var host = builder.Build();
await host.RunAsync();
