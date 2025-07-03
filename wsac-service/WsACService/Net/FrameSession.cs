using WsACService.IO.Abstractions;
using WsACService.Net.FrameHandlers;
using WsACService.Net.Models;

namespace WsACService.Net;

public class FrameSession(ILogger logger, SessionState state, IWriter writer, IReader reader)
{
    public SessionState State { get; } = state;

    public async Task RunAsync(CancellationToken ct)
    {
        try
        {
            await RunAsyncInternal(ct);
        }
        catch (EndOfStreamException)
        {
            logger.LogError("unexpected EOF");
        }
        catch (OperationCanceledException)
        {
            logger.LogError("operation cancelled");
        }
    }

    private async Task HandleFrameAsync(FrameHeader header, IReader body, CancellationToken ct)
    {
        var handler = FrameHandler.All.FirstOrDefault(handler => handler.IsTarget(header.Signature));
        if (handler is null)
        {
            // TODO : make event
            logger.LogWarning("unhandled frame (signature: 0x{:X})", header.Signature);
            await body.SkipAsync(header.DataSize, ct);
            return;
        }

        if (State.FrameSessionState == FrameSessionState.NONE && handler is not CheckpointHandler)
        {
            logger.LogWarning("regular data on NONE state; skipping...");
            await body.SkipAsync(header.DataSize, ct);
            return;
        }

        await handler.HandleAsync(this, body, ct);

        if (body.Available != 0)
        {
            // TODO : make event
            logger.LogWarning("handler only consumes partial body");
            await body.SkipAsync(body.Available, ct);
        }
    }

    private async Task RunAsyncInternal(CancellationToken ct)
    {
        var preamble = new Preamble();
        
        while (!ct.IsCancellationRequested)
        {
            if (!ReceivePreamble(ref preamble, ct))
            {
                // TODO : make event
                logger.LogWarning("broken preamble");
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

        logger.LogInformation("session closed successfully");
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

    private bool ReceivePreamble(ref Preamble preamble, CancellationToken ct)
    {
        preamble.Push(reader.Read<byte>(ct));
        return preamble == Config.Preamble;
    }
}