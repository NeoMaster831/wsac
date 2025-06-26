using System.IO.Pipes;
using System.Runtime.InteropServices;

namespace WsACService;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct PipeRcvPayloadHeader
{
    public ulong Signature;
    public ulong RequestId;
    public ulong Id;
    public int Status;
    public ulong Timestamp;
    public int BodySize;

    public override string ToString()
    {
        return $"{nameof(PipeRcvPayloadHeader)}{{req-id:{RequestId}, id:{Id}, status:{Status}, timestamp:{Timestamp}, bodysize:{BodySize}}}";
    }
}

internal class ConnectionClosedException : Exception;

public sealed class PipeServer : BackgroundService
{
    private static int _currentId;

    private readonly ILogger _logger;
    private readonly NamedPipeServerStream _pipe;

    private int Id { get; }

    public PipeServer(int nThread, ILogger logger)
    {
        Id = Interlocked.Increment(ref _currentId);
        _logger = logger;
        
        LogInfo("pipe opening...");
        _pipe = new NamedPipeServerStream("WsACPipe", PipeDirection.InOut, nThread, PipeTransmissionMode.Byte,
            PipeOptions.Asynchronous);
        LogInfo("pipe opened");
    }

    private void LogInfo(string message)
    {
        _logger.LogInformation("[PipeServer {I}] {Message}", Id, message);
    }

    private void LogError(string message, Exception e)
    {
        _logger.LogError(e, "[PipeServer {I}] {Message}", Id, message);
    }

    protected override async Task ExecuteAsync(CancellationToken ct)
    {
        LogInfo("started");
        
        while (!ct.IsCancellationRequested)
        {
            await ProcessPipeInteraction(ct);
        }
        
        LogInfo("stopped");
    }

    private async Task ProcessPipeInteraction(CancellationToken ct)
    {
        try
        {
            LogInfo("wait for connection...");
            await _pipe.WaitForConnectionAsync(ct);
            LogInfo("connection established");
            ProcessClientMessages(ct);
        }
        catch (Exception ex)
        {
            switch (ex)
            {
                case ConnectionClosedException:
                    LogInfo("connection closed peacefully");
                    break;
                
                case EndOfStreamException:
                    LogInfo("connection closed unexpectedly (EOF)");
                    break;
                
                case OperationCanceledException:
                    LogInfo("operation canceled");
                    break;
                
                case IOException:
                    LogInfo("IO failed unexpectedly (client-side fault)");
                    await Task.Delay(100, CancellationToken.None);
                    break;
                
                default:
                    LogError("unexpected exception", ex);
                    await Task.Delay(1000, ct);
                    break;
            }
        }
        finally
        {
            LogInfo("pipe closing...");
            if (_pipe.IsConnected)
                _pipe.Disconnect();
            LogInfo("pipe closed");
        }
    }

    private void ProcessClientMessages(CancellationToken ct)
    {
        while (!ct.IsCancellationRequested && _pipe.IsConnected)
        {
            var header = _pipe.Read<PipeRcvPayloadHeader>(ct);
            
            LogInfo(header.ToString());
            
            if (header.Signature != Config.PayloadSignature)
                return;

            if (header.Id == Config.PeacefulExit)
                throw new ConnectionClosedException();

            if (header.BodySize <= 0) continue;
            using var body = _pipe.Read(header.BodySize, ct);
            LogInfo($"body: {body.Memory.Length} bytes");
        }
    }

    public override void Dispose()
    {
        _pipe.Close();
        base.Dispose();
    }
}