using WsACService.IO.Abstractions;

namespace WsACService.Net;

public partial struct PacketSession
{
    [Obsolete("do NOT use default ctor of this struct")]
    public PacketSession() => throw new NotImplementedException();
    
    public PacketSession(ILogger logger, SessionState state, IReader reader)
    {
        
    }
    
    public async ValueTask HandleAsync(IReader reader, CancellationToken ct)
    {
        
    }
}