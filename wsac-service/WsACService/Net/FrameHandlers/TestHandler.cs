using WsACService.IO.Abstractions;
using WsACService.Net.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.FrameHandlers;

[FrameHandler]
public class TestHandler : IFrameHandler
{
    public bool IsTarget(FrameSignature signature) => signature is FrameSignature.Test;

    public async Task HandleAsync(FrameSession session, IReader body, CancellationToken ct)
    {
        var size = body.Available;
        await body.SkipAsync(size, ct);
        
        session.State
            .CreateLogger(nameof(TestHandler))
            .LogWarning("test frame received ({} bytes)", size);
    }
}
