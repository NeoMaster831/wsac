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

    public Task HandleAsync(Session session, Frame frame, CancellationToken ct)
    {
        throw new NotImplementedException();
    }
}
