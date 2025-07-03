using System.Runtime.InteropServices;
using System.Security.Cryptography;

namespace WsACService.Net.Models;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct FrameHeader
{
    public readonly     FrameSignature Signature;
    public readonly     long           DataSize;
    public unsafe fixed byte           Nonce[12];
    public unsafe fixed byte           MAC[16];

    public FrameHeader(FrameSignature signature, long size = 0)
    {
        Signature = signature;
        DataSize  = size;
        Sign();
    }

    private void Sign()
    {
        Span<byte> buffer = stackalloc byte[10];
        MemoryMarshal.Write(buffer.Slice(0, 2), in Signature);
        MemoryMarshal.Write(buffer.Slice(2, 8), in DataSize);

        unsafe
        {
            fixed (byte* pNonce = Nonce)
            fixed (byte* pMAC = MAC)
            {
                var nonce = new Span<byte>(pNonce, 12);
                var mac   = new Span<byte>(pMAC, 16);

                using var cipher = new ChaCha20Poly1305(Config.PSK[0]);
                RandomNumberGenerator.Fill(nonce);

                cipher.Encrypt(nonce, ReadOnlySpan<byte>.Empty, Span<byte>.Empty, mac, buffer);
            }
        }
    }

    public bool Validate()
    {
        unsafe
        {
            var signed = this;
            signed.Sign();

            fixed (byte* pNonce = Nonce)
            fixed (byte* pMAC = MAC)
            {
                var nonce  = new Span<byte>(pNonce, 12);
                var oNonce = new Span<byte>(signed.Nonce, 12);

                var mac  = new Span<byte>(pMAC, 16);
                var oMAC = new Span<byte>(signed.MAC, 12);

                return nonce.SequenceEqual(oNonce) && mac.SequenceEqual(oMAC);
            }
        }
    }
}