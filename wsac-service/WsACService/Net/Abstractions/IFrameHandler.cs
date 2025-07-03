using WsACService.IO;
using WsACService.IO.Abstractions;
using WsACService.Net.Models;

namespace WsACService.Net.Abstractions;

public interface IFrameHandler
{
    public bool IsTarget(FrameSignature signature);

    public Task HandleAsync(FrameSession session, IReader body, FrameWriter writer, CancellationToken ct);
}