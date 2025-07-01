using WsACService.InterProcess.Abstractions;
using WsACService.InterProcess.Models;
using WsACService.Logging;

namespace WsACService.InterProcess;

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
