using WsACService.InProcess;
using WsACService.IO.Abstractions;
using WsACService.Logging;
using WsACService.Net.FrameHandlers;
using WsACService.Net.Models;

namespace WsACService.Net;

public class FrameSession(int id, ILogger logger, ILowLevelWriter writer, ILowLevelReader reader)
{
    public int     Id     { get; } = id;
    public ILogger Logger { get; } = logger;

    public FrameSessionState State { get; set; } = FrameSessionState.READY;

    public PacketSession NextSession { get; } = new();

    public async Task RunAsync(CancellationToken ct)
    {
        try
        {
            await RunAsyncInternal(ct);
        }
        catch (EndOfStreamException)
        {
            Logger.Error(Id, "unexpected EOF");
        }
        catch (OperationCanceledException)
        {
            Logger.Error(Id, "operation cancelled");
        }
    }

    private async Task HandleFrameAsync(FrameHeader header, ILowLevelReader body, CancellationToken ct)
    {
        var handler = FrameHandler.All.FirstOrDefault(handler => handler.IsTarget(header.Signature));
        if (handler is null)
        {
            // TODO : make event
            Logger.Warn(Id, $"unhandled frame (signature: 0x{header.Signature:X})");
            await body.SkipAsync(header.DataSize, ct);
            return;
        }

        if (State == FrameSessionState.NONE && handler is not CheckpointHandler)
        {
            Logger.Warn(Id, "regular data on NONE state; skipping...");
            await body.SkipAsync(header.DataSize, ct);
            return;
        }

        await handler.HandleAsync(this, header, body, ct);

        if (body.Available != 0)
        {
            // TODO : make event
            Logger.Warn(Id, "handler only consumes partial body");
            await body.SkipAsync(body.Available, ct);
        }
    }

    private async Task RunAsyncInternal(CancellationToken ct)
    {
        var preamble = new byte[Consts.Preamble.Length];

        while (!ct.IsCancellationRequested)
        {
            if (!ReceivePreamble(preamble, ct))
            {
                // TODO : make event
                Logger.Warn(Id, "broken preamble");
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

        Logger.Info(Id, "session closed successfully");
    }

    public void SendCheckpoint()
    {
        var initialResetFrame = new FrameHeader(FrameSignature.Checkpoint, 0);
        writer.Write(initialResetFrame);
    }

    private void RequestCheckpoint()
    {
        SendCheckpoint();
        State = FrameSessionState.NONE;
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
        return buffer.SequenceEqual(Consts.Preamble);
    }
}