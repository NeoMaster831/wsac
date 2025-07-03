using WsACService.IO;
using WsACService.IO.Abstractions;
using WsACService.Net.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.FrameHandlers;

[FrameHandler]
public class RegularHandler : IFrameHandler
{
    public bool IsTarget(FrameSignature signature) => signature is FrameSignature.Regular;

    public async Task HandleAsync(FrameSession session, IReader body, FrameWriter writer, CancellationToken ct)
    {
        var logger = session.State.CreateLogger(nameof(PacketSession));
        await new PacketSession(logger, session.State, body, writer).HandleAsync(body, ct);
    }
}
