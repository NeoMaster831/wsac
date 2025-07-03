using WsACService.IO.Abstractions;
using WsACService.Net.FrameHandlers;
using WsACService.Net.Models;

namespace WsACService.Net;

public class FrameSession(ILogger logger, SessionState state, IWriter writer, IReader reader)
{
    public ILogger      Logger { get; } = logger;
    public SessionState State  { get; } = state;

    public async Task RunAsync(CancellationToken ct)
    {
        try
        {
            await RunAsyncInternal(ct);
        }
        catch (EndOfStreamException)
        {
            Logger.LogError("unexpected EOF");
        }
        catch (OperationCanceledException)
        {
            Logger.LogError("operation cancelled");
        }
    }

    private async Task HandleFrameAsync(FrameHeader header, IReader body, CancellationToken ct)
    {
        var handler = FrameHandler.All.FirstOrDefault(handler => handler.IsTarget(header.Signature));
        if (handler is null)
        {
            // TODO : make event
            Logger.LogWarning("unhandled frame (signature: 0x{:X})", header.Signature);
            await body.SkipAsync(header.DataSize, ct);
            return;
        }

        if (State.FrameSessionState == FrameSessionState.NONE && handler is not CheckpointHandler)
        {
            Logger.LogWarning("regular data on NONE state; skipping...");
            await body.SkipAsync(header.DataSize, ct);
            return;
        }

        await handler.HandleAsync(this, body, ct);

        if (body.Available != 0)
        {
            // TODO : make event
            Logger.LogWarning("handler only consumes partial body");
            await body.SkipAsync(body.Available, ct);
        }
    }

    private async Task RunAsyncInternal(CancellationToken ct)
    {
        var preamble = new byte[Config.Preamble.Length];

        while (!ct.IsCancellationRequested)
        {
            if (!ReceivePreamble(preamble, ct))
            {
                // TODO : make event
                Logger.LogWarning("broken preamble");
                continue;
            }

            if (!ReadHeader(out var header, ct))
            {
                // TODO : make event
                RequestCheckpoint();
                continue;
            }

            var bodyReader = reader.CreateChild(header.DataSize);
            await HandleFrameAsync(header, bodyReader, ct);
        }

        Logger.LogInformation("session closed successfully");
    }

    public void SendCheckpoint()
    {
        var initialResetFrame = new FrameHeader(FrameSignature.Checkpoint, 0);
        writer.Write(initialResetFrame);
    }

    private void RequestCheckpoint()
    {
        SendCheckpoint();
        State.FrameSessionState = FrameSessionState.NONE;
    }

    private bool ReadHeader(out FrameHeader header, CancellationToken ct)
    {
        header = reader.Read<FrameHeader>(ct);
        if (!header.Validate())
            return false;

        if (header.DataSize < 0)
            throw new InsufficientMemoryException($"Peer sends too big payload ({header.DataSize} bytes)");

        return true;
    }

    private bool ReceivePreamble(Span<byte> buffer, CancellationToken ct)
    {
        // P[P0, P1, ... Pn-1, Pn] -> P[P1, P2, ... Pn, Pn]
        for (var i = 0; i < buffer.Length - 1; ++i)
            buffer[i] = buffer[i + 1];

        // P[P0, P1, ... Pn-1, Pn] -> P[P1, P2, ... Pn, Pn+1]
        buffer[^1] = reader.Read<byte>(ct);

        // P == K?
        return buffer.SequenceEqual(Config.Preamble);
    }
}