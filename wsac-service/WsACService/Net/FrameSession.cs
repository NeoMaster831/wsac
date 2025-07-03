using WsACService.IO;
using WsACService.IO.Abstractions;
using WsACService.Net.Abstractions;
using WsACService.Net.FrameHandlers;
using WsACService.Net.Models;

namespace WsACService.Net;

public class FrameSession(ILogger logger, SessionState state, IWriter writer, IReader reader)
{
    public SessionState State { get; } = state;

    private FrameWriter FrameWriter { get; } = new(writer);

    public async Task RunAsync(CancellationToken ct)
    {
        logger.LogInformation("starting session");
        try
        {
            await RunAsyncInternal(ct);
        }
        catch (EndOfStreamException)
        {
            logger.LogError("unexpected EOF");
        }
        catch (IOException e)
        {
            logger.LogError("connection refused by peer: {}", e.Message);
        }
        catch (OperationCanceledException)
        {
            logger.LogError("operation cancelled");
        }
        catch (Exception ex)
        {
            logger.LogError(ex, "unhandled exception");
        }
    }

    private async Task HandleFrameAsync(FrameHeader header, IReader body, CancellationToken ct)
    {
        IFrameHandler? handler = null;
        foreach (var candidate in FrameHandler.All)
        {
            if (!candidate.IsTarget(header.Signature))
                continue;
            handler = candidate;
            break;
        }

        if (handler is null)
        {
            // TODO : make event
            logger.LogWarning("unhandled frame (signature: 0x{:X})", header.Signature);
            await body.SkipAsync(header.DataSize, ct);
            return;
        }

        if (State.FrameSessionState == FrameSessionState.None && handler is RegularHandler)
        {
            logger.LogWarning("regular data on NONE state; skipping...");
            await body.SkipAsync(header.DataSize, ct);
            return;
        }

        await handler.HandleAsync(this, body, FrameWriter, ct);

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
            var foundPreamble = false;
            for (var i = 0; i < Preamble.Size; ++i)
                foundPreamble |= ReceivePreamble(ref preamble, ct);

            if (!foundPreamble)
            {
                // TODO : make event
                logger.LogWarning("received preamble {A} differs to {B}", preamble, Config.Preamble);
                continue;
            }

            logger.LogDebug("preamble received");

            if (!ReadHeader(out var header, ct))
            {
                // TODO : make event
                logger.LogError("broken header integrity; requesting checkpoint...");

                logger.LogDebug("received:\n{}", header);
                logger.LogDebug("expected:\n{}", header.Sign());

                RequestCheckpoint();
                continue;
            }

            logger.LogDebug("header received (sig: {sig}, size: {siz})", header.Signature, header.DataSize);

            var bodyReader = reader.CreateChild(header.DataSize);
            await HandleFrameAsync(header, bodyReader, ct);
        }

        logger.LogInformation("session closed successfully");
    }

    private void RequestCheckpoint()
    {
        var frame = new FrameHeader(FrameSignature.Checkpoint);
        FrameWriter.Write(frame);
        State.FrameSessionState = FrameSessionState.None;
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