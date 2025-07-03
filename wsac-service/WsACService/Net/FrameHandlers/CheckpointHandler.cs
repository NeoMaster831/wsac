using WsACService.IO;
using WsACService.IO.Abstractions;
using WsACService.Net.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.FrameHandlers;

[FrameHandler]
public class CheckpointHandler : IFrameHandler
{
    public bool IsTarget(FrameSignature signature) => signature is FrameSignature.Checkpoint;

    public Task HandleAsync(FrameSession session, IReader body, FrameWriter writer, CancellationToken ct)
    {
        if (session.State.FrameSessionState is FrameSessionState.None)
            session.State.FrameSessionState = FrameSessionState.Ready;
        else
            writer.Write(new FrameHeader(FrameSignature.Checkpoint));

        return Task.CompletedTask;
    }
}
