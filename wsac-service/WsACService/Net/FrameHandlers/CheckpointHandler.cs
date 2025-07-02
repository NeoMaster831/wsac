using WsACService.IO.Abstractions;
using WsACService.Net.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.FrameHandlers;

[FrameHandler]
public class CheckpointHandler : IFrameHandler
{
    public bool IsTarget(FrameSignature signature) => signature is FrameSignature.Checkpoint;

    public Task HandleAsync(FrameSession session, FrameHeader header, ILowLevelReader body, CancellationToken ct)
    {
        if (session.State is FrameSessionState.NONE)
            session.State = FrameSessionState.READY;
        else
            session.SendCheckpoint();

        return Task.CompletedTask;
    }
}
