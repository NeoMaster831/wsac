using System.IO.Pipes;
using WsACService.InterProcess;
using WsACService.InterProcess.Pipe;

namespace WsACService.Services;

public sealed class PipeSessionService : IHostedService, IDisposable
{
    private static int _sessionId;
    
    private readonly CancellationTokenSource _cts = new();
    private readonly Thread[]                _threads;

    private readonly ILogger    _logger;

    public PipeSessionService(ILogger<PipeSessionService> logger)
    {
        _logger = logger;

        _threads = new Thread[Consts.NumThread];
        for (var i = 0; i < Consts.NumThread; i++)
        {
            _threads[i] = new Thread(RunThread);
        }
    }

    private void RunThread()
    {
        var ct = _cts.Token;

        using var pipe = new NamedPipeServerStream(Consts.PipeName, PipeDirection.InOut, Consts.NumThread);

        var reader = new PipeLowLevelReader(pipe);
        var writer = new PipeLowLevelWriter(pipe);
        
        while (!ct.IsCancellationRequested)
        {
            pipe.WaitForConnection();
            
            var id = Interlocked.Increment(ref _sessionId);
            var task = new FrameSession(id, _logger, writer, reader).RunAsync(ct);
            
            task.GetAwaiter().GetResult();
        }
    }

    public Task StartAsync(CancellationToken ct)
    {
        for (var i = 0; i < Consts.NumThread; i++)
        {
            _threads[i].Start();
        }

        return Task.CompletedTask;
    }

    public Task StopAsync(CancellationToken ct)
    {
        _cts.Cancel();
        
        for (var i = 0; i < Consts.NumThread; i++)
        {
            _threads[i].Join();
        }

        return Task.CompletedTask;
    }

    public void Dispose()
    {
        _cts.Dispose();
    }
}