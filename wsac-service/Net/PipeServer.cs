using System.IO.Pipes;
using WsACService.Logging;

namespace WsACService.Net;

internal class SuspiciousPayloadException : Exception; // FIX

public sealed class PipeServer : BackgroundService, ILoggable
{
    private static int _currentId;

    private readonly NamedPipeServerStream _pipe;
    private readonly CryptoProvider _cryptoProvider;

    public int Identifier { get; }
    public ILogger Logger { get; }

    public PipeServer(int nThread, ILogger logger)
    {
        Identifier = Interlocked.Increment(ref _currentId);
        Logger = logger;
        _cryptoProvider = new CryptoProvider();

        this.Info("pipe opening...");
        _pipe = new NamedPipeServerStream("WsACPipe", PipeDirection.InOut, nThread, PipeTransmissionMode.Byte,
            PipeOptions.Asynchronous);
        this.Info("pipe opened");
    }

    protected override async Task ExecuteAsync(CancellationToken ct)
    {
        this.Info("started");

        while (!ct.IsCancellationRequested)
        {
            await ProcessPipeInteraction(ct);
        }

        this.Info("stopped");
    }

    private async Task ProcessPipeInteraction(CancellationToken ct)
    {
        try
        {
            this.Info("wait for connection...");
            await _pipe.WaitForConnectionAsync(ct);
            this.Info("connection established");
            ProcessClientMessages(ct);
        }
        catch (Exception ex)
        {
            switch (ex)
            {
                case ConnectionClosedException:
                    this.Info("connection closed peacefully");
                    break;

                case EndOfStreamException:
                    this.Info("connection closed unexpectedly (EOF)");
                    break;

                case OperationCanceledException:
                    this.Info("operation canceled");
                    break;

                case IOException:
                    this.Info("IO failed unexpectedly (client-side fault)");
                    await Task.Delay(100, CancellationToken.None);
                    break;

                default:
                    this.Error("unexpected exception", ex);
                    await Task.Delay(1000, ct);
                    break;
            }
        }
        finally
        {
            this.Info("pipe closing...");
            if (_pipe.IsConnected)
                _pipe.Disconnect();
            this.Info("pipe closed");
        }
    }

    private void ProcessClientMessages(CancellationToken ct)
    {
        while (!ct.IsCancellationRequested && _pipe.IsConnected)
        {
            switch (_pipe.Read<ulong>(ct) /* signature */)
            {
                /* Before key exchange */

                case Config.KeyExchangeSignature when !_cryptoProvider.IsKeyExchanged:
                {
                    var keySize = _pipe.Read<int>(ct);
                    using var key = _pipe.Read(keySize, ct);

                    var response = _cryptoProvider.GenerateResponse(key.Memory[..keySize].ToArray());
                    var header = new PipeCryptoExchangeResponseHeader
                    {
                        Signature = Config.KeyExchangeSignature,
                        KeySize = response.Length
                    };
                    _pipe.Write(header);
                    _pipe.Write(response);

                    break;
                }

                /* After key exchange */

                case Config.PayloadSignature when _cryptoProvider.IsKeyExchanged:
                    break;

                default:
                    throw new SuspiciousPayloadException();
            }
        }
    }

    public override void Dispose()
    {
        _pipe.Close();
        base.Dispose();
    }
}