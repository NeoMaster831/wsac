using WsACService.InterProcess.Models;

namespace WsACService.InterProcess.Abstractions;

public interface IFrameHandler
{
    public bool IsTarget(Signature signature);

    public Task HandleAsync(FrameSession session, FrameHeader header, Stream body, CancellationToken ct);
}