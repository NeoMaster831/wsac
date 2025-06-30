using System.Runtime.InteropServices;

namespace WsACService;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct PipeCryptoExchangeResponseHeader
{
    public ulong Signature;
    public int KeySize;
}
