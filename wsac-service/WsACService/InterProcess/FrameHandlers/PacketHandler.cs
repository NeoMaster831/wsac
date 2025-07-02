using WsACService.InterProcess.Abstractions;
using WsACService.InterProcess.Models;

namespace WsACService.InterProcess.FrameHandlers;

[FrameHandler]
public class PacketHandler : IFrameHandler
{
    public bool IsTarget(FrameSignature signature)
    {
        return signature is FrameSignature.Regular;
    }

    public Task HandleAsync(FrameSession session, FrameHeader header, Stream body, CancellationToken ct)
    {
        return session.NextSession.HandleAsync(body, ct);
    }
}
