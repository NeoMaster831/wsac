using WsACService.InterProcess;
using WsACService.Layers.InterProcess.Abstractions;
using WsACService.Layers.InterProcess.Models;

namespace WsACService.Layers.InterProcess.Handlers;

[FrameHandler]
public class ResetHandler : IFrameHandler
{
    public bool IsTarget(Signature signature)
    {
        return signature is Signature.RESET;
    }

    public Task HandleAsync(FrameSession session, Frame frame, CancellationToken ct)
    {
        session.StateStore.State = State.READY;
        return Task.CompletedTask;
    }
}
