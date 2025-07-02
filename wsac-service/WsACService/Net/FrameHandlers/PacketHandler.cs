using WsACService.IO.Abstractions;
using WsACService.Net.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.FrameHandlers;

[FrameHandler]
public class PacketHandler : IFrameHandler
{
    public bool IsTarget(FrameSignature signature)
    {
        return signature is FrameSignature.Regular;
    }

    public Task HandleAsync(FrameSession session, FrameHeader header, ILowLevelReader body, CancellationToken ct)
    {
        return session.NextSession.HandleAsync(body, ct);
    }
}
