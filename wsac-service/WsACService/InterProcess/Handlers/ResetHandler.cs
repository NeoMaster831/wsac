using WsACService.InterProcess.Abstractions;
using WsACService.InterProcess.Models;

namespace WsACService.InterProcess.Handlers;

[FrameHandler]
public class ResetHandler : IFrameHandler
{
    public bool IsTarget(Signature signature)
    {
        return signature is Signature.RESET;
    }

    public Task HandleAsync(Session session, Frame frame, CancellationToken ct)
    {
        session.StateStore.State = State.READY;
        return Task.CompletedTask;
    }
}
