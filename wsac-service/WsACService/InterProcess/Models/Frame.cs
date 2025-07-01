using WsACService.Memory;

namespace WsACService.InterProcess.Models;

public readonly struct Frame
{
    public readonly Signature     Signature;
    public readonly long          DataSize;
    public readonly MemoryBuffer? Data;

    public Frame(Signature signature)
    {
        Signature = signature;
        DataSize  = 0;
        Data      = null;
    }
    
    public Frame(Signature signature, long dataSize, MemoryBuffer data)
    {
        ArgumentOutOfRangeException.ThrowIfGreaterThan(dataSize, data.Length);

        Signature = signature;
        DataSize  = dataSize;
        Data = data;
    }
}
