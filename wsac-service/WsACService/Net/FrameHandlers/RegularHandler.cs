using WsACService.IO.Abstractions;
using WsACService.Net.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.FrameHandlers;

[FrameHandler]
public class RegularHandler : IFrameHandler
{
    public bool IsTarget(FrameSignature signature) => signature is FrameSignature.Regular;

    public async Task HandleAsync(FrameSession session, ILowLevelReader body, CancellationToken ct)
    {
        await new PacketSession(session.Logger, session.State, body).HandleAsync(body, ct);
    }
}
