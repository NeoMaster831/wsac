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
        if (session.State.FrameSessionState is FrameSessionState.None)
            session.State.FrameSessionState = FrameSessionState.Ready;
        else
            session.SendCheckpoint();

        return Task.CompletedTask;
    }
}
