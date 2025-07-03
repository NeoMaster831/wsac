using WsACService.IO;
using WsACService.IO.Abstractions;

namespace WsACService.Net;

public readonly struct PacketSession
{
    private readonly ILogger      _logger;
    private readonly SessionState _state;
    private readonly IReader      _reader;
    private readonly FrameWriter  _writer;

    [Obsolete("do NOT use default ctor of this struct")]
    public PacketSession() => throw new NotImplementedException();

    public PacketSession(ILogger logger, SessionState state, IReader reader, FrameWriter writer)
    {
        _logger = logger;
        _state  = state;
        _reader = reader;
        _writer = writer;
    }

    public async ValueTask HandleAsync(IReader reader, CancellationToken ct)
    {
    }
}