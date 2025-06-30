using System.Buffers;
using System.IO.Pipes;
using System.Runtime.InteropServices;
using WsACService.Net;

namespace WsACService;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct PipeRcvPayloadHeader
{
    public ulong RequestId;
    public ulong Id;
    public int Status;
    public ulong Timestamp;
    public int BodySize;

    public override string ToString()
    {
        return $"{nameof(PipeRcvPayloadHeader)}{{req-id:{RequestId}, id:{Id}, status:{Status}, timestamp:{Timestamp}, bodysize:{BodySize}}}";
    }
}

internal class ConnectionClosedException : Exception;

public static class RcvPayloadManager
{
    public static void Perform(PipeServer server, CancellationToken ct)
    {
        var header = server.Read<PipeRcvPayloadHeader>(ct);
        if (header.Id == Config.PeacefulExit)
            throw new ConnectionClosedException();


        using var body = pipe.Read(header.BodySize, ct);
        
        
    }
}