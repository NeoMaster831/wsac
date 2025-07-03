using WsACService.IO.Abstractions;

namespace WsACService.Net;

public partial struct PacketSession
{
    [Obsolete("do NOT use default ctor of this struct")]
    public PacketSession() => throw new NotImplementedException();
    
    public PacketSession(ILogger logger, SessionState state, ILowLevelReader reader)
    {
        
    }
    
    public async ValueTask HandleAsync(ILowLevelReader reader, CancellationToken ct)
    {
        
    }
}