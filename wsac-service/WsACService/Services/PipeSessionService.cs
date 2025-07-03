using System.IO.Pipes;
using WsACService.IO;
using WsACService.Logging;
using WsACService.Net;

namespace WsACService.Services;

public sealed class PipeSessionService : IHostedService, IDisposable
{
    private static int _sessionId;

    private readonly CancellationTokenSource _cts = new();
    private readonly Thread[]                _threads;

    public PipeSessionService()
    {
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

        var reader = new PipeReader(pipe);
        var writer = new PipeWriter(pipe);

        var thread = Environment.CurrentManagedThreadId;
        var logger = ConsoleLoggerProvider.Instance.CreateLogger($"PipeSessionService:{thread}");

        while (!ct.IsCancellationRequested)
        {
            logger.LogInformation("waiting for connection...");

            try
            {
                pipe.WaitForConnectionAsync(ct)
                    .GetAwaiter()
                    .GetResult();
            }
            catch (OperationCanceledException)
            {
                logger.LogInformation("waiting canceled; exiting...");
                break;
            }

            logger.LogInformation("pipe connection established");

            var id            = Interlocked.Increment(ref _sessionId);
            var sessionLogger = ConsoleLoggerProvider.Instance.CreateLogger($"{nameof(FrameSession)}:{id}");
            var state         = new SessionState(id);
            var task          = new FrameSession(sessionLogger, state, writer, reader).RunAsync(ct);

            task.GetAwaiter().GetResult();

            logger.LogInformation("pipe connection closed");
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