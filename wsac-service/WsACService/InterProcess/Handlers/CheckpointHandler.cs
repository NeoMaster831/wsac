using WsACService.InterProcess.Abstractions;
using WsACService.InterProcess.Models;

namespace WsACService.InterProcess.Handlers;

[FrameHandler]
public class CheckpointHandler : IFrameHandler
{
    public bool IsTarget(Signature signature) => signature is Signature.CHECKPOINT;

    public Task HandleAsync(FrameSession session, FrameHeader header, Stream body, CancellationToken ct)
    {
        if (session.State is FrameSessionState.NONE)
            session.State = FrameSessionState.READY;
        else
            session.SendCheckpoint();

        return Task.CompletedTask;
    }
}
