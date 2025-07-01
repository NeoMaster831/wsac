using WsACService.Layers.InterProcess.Models;

namespace WsACService.Layers.InterProcess.Abstractions;

public interface IFrameHandler
{
    public bool IsTarget(Signature signature);

    public Task HandleAsync(FrameSession session, Frame frame, CancellationToken ct);
}