using WsACService.IO.Abstractions;
using WsACService.Net.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.FrameHandlers;

[FrameHandler]
public class CheckpointHandler : IFrameHandler
{
    public bool IsTarget(FrameSignature signature) => signature is FrameSignature.Checkpoint;

    public Task HandleAsync(FrameSession session, IReader body, CancellationToken ct)
    {
        if (session.State.FrameSessionState is FrameSessionState.NONE)
            session.State.FrameSessionState = FrameSessionState.READY;
        else
            session.SendCheckpoint();

        return Task.CompletedTask;
    }
}
