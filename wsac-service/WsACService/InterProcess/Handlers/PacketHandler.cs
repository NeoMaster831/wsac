using WsACService.InterProcess.Abstractions;
using WsACService.InterProcess.Models;

namespace WsACService.InterProcess.Handlers;

[FrameHandler]
public class PacketHandler : IFrameHandler
{
    public bool IsTarget(Signature signature)
    {
        return signature is Signature.REGULAR;
    }

    public Task HandleAsync(FrameSession session, FrameHeader header, Stream body, CancellationToken ct)
    {
        return session.NextSession.HandleAsync(body, ct);
    }
}
