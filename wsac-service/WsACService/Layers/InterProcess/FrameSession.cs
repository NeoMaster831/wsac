using WsACService.Layers.InterProcess.Abstractions;
using WsACService.Layers.InterProcess.Handlers;
using WsACService.Layers.InterProcess.Models;
using WsACService.Logging;
using WsACService.Memory;

namespace WsACService.Layers.InterProcess;

public class FrameSession(int id, ILogger logger, ILowLevelWriter writer, ILowLevelReader reader)
{
    public int     Id     { get; } = id;
    public ILogger Logger { get; } = logger;
    public FrameWriter Writer { get; } = new(writer);
    public StateStore StateStore { get; } = new();

    
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

    private async Task RequestResetAsync(CancellationToken ct)
    {
        var initialResetFrame = new Frame(Signature.RESET);
        await Writer.WriteAsync(initialResetFrame, ct);
        StateStore.State = State.NONE;
    }

    private async Task RunAsyncInternal(CancellationToken ct)
    {
        var buffer   = new MemoryBuffer();
        var preamble = new byte[Consts.Preamble.Length];

        await RequestResetAsync(ct);

        while (!ct.IsCancellationRequested) // TODO : state check
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
                await RequestResetAsync(ct);
                continue;
            }

            await ReadBodyAsync(buffer, header, ct);

            var handled = false;
            foreach (var handler in FrameHandler.All)
            {
                if (!handler.IsTarget(header.Signature))
                    continue;

                var isNone  = StateStore.State == State.NONE;
                var isReset = handler is ResetHandler;
                if (isNone && !isReset)
                {
                    // TODO : make event
                    Logger.Warn(Id, "regular signal on NONE state; skipping...");
                    break;
                }
                if (!isNone && isReset)
                {
                    // TODO : make event
                    Logger.Warn(Id, "reset signal on READY state; skipping...");
                    break;
                }

                var frame = new Frame(
                    header.Signature,
                    header.DataSize,
                    buffer
                );
                await handler.HandleAsync(this, frame, ct);
                handled = true;
            }

            if (!handled)
            {
                // TODO : make event
                Logger.Warn(Id, $"unhandled frame (signature: 0x{header.Signature:X})");
            }
        }

        Logger.Info(Id, "session closed successfully");
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

    private async Task ReadBodyAsync(MemoryBuffer buffer, FrameHeader header, CancellationToken ct)
    {
        if (header.Signature != Signature.RESET)
        {
            buffer.Reserve(header.DataSize);
            await reader.ReadAsync(buffer, header.DataSize, ct);
        }
        else if (header.DataSize > 0)
        {
            // TODO : make event
            Logger.Warn(Id, "reset frame has data; skipping...");
            await reader.SkipAsync(header.DataSize, ct);
        }
    }

    private bool ReceivePreamble(Span<byte> buffer, CancellationToken ct)
    {
        // P[P0, P1, ... Pn-1, Pn] -> P[P1, P2, ... Pn, Pn]
        for (var i = 0; i < buffer.Length - 1; ++i)
            buffer[i] = buffer[i + 1];

        // P[P0, P1, ... Pn-1, Pn] -> P[P1, P2, ... Pn, Pn+1]
        reader.Read(buffer.Slice(5, 1), ct);

        // P == K?
        return buffer.SequenceEqual(Consts.Preamble);
    }
}
