using WsACService.InterProcess.Models;

namespace WsACService.InterProcess.Abstractions;

public interface IFrameHandler
{
    public bool IsTarget(Signature signature);

    public Task HandleAsync(FrameSession session, Frame frame, CancellationToken ct);
}