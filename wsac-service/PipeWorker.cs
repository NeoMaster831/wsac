namespace WsACService;

public sealed class PipeWorker(ILogger<PipeManager> logger, PipeManager manager) : IHostedService
{
    public async Task StartAsync(CancellationToken ct)
    {
        for (var i = 0; i < PipeManager.NumThread; i++)
        {
            manager.Workers[i] = new PipeServer(PipeManager.NumThread, logger);
            await manager.Workers[i].StartAsync(ct);
        }
        
        manager.NotifyInit();
    }

    public async Task StopAsync(CancellationToken ct)
    {
        for (var i = 0; i < PipeManager.NumThread; i++)
            await manager.Workers[i].StopAsync(ct);
        var tasks = manager.Workers
            .Select(worker => worker.ExecuteTask)
            .Where(task => task is not null);
        await Task.WhenAll(tasks!);
    }
}
