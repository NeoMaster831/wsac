using WsACService.InterProcess.Models;

namespace WsACService.InterProcess.Abstractions;

public interface IFrameHandler
{
    public bool IsTarget(Signature signature);

    public Task HandleAsync(Session session, Frame frame, CancellationToken ct);
}