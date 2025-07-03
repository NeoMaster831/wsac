using WsACService.IO.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.Abstractions;

public interface IFrameHandler
{
    public bool IsTarget(FrameSignature signature);

    public Task HandleAsync(FrameSession session, ILowLevelReader body, CancellationToken ct);
}